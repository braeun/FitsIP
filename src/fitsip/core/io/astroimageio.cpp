/********************************************************************************
 *                                                                              *
 * FitsIP - astro image format reader                                           *
 *                                                                              *
 * modified: 2025-11-03                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of FitsIP.                                                 *
 * FitsIP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * FitsIP is distributed in the hope that it will be useful, but                *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * FitsIP. If not, see <https://www.gnu.org/licenses/>.                         *
 ********************************************************************************/

#include "astroimageio.h"
#include "../fitsimage.h"
#include "../fitsobject.h"
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QDebug>
#include <stdexcept>

static const char* ai_magic = "AIMG";
static const uint32_t ai_version = 0x0100;

#define KEY_LEN 16
#define VALUE_LEN 128

static const char* ai_key_object = "object";
static const char* ai_key_observer = "observer";
static const char* ai_key_instrument = "instrument";
static const char* ai_key_telescope = "telescope";

const char* AstroImageIO::FILENAME_FILTER = "Internal Astro Image (*.aimg)";

AstroImageIO::AstroImageIO()
{
}

AstroImageIO::~AstroImageIO()
{
}

std::vector<std::shared_ptr<FitsObject>> AstroImageIO::read(QString filename)
{
  QFileInfo info(filename);
  profiler.start();
  QFile f(filename);
  if (!f.open(QFile::ReadOnly)) throw std::runtime_error("AstroImageIO: Failed to open file");
  QByteArray a = f.read(4);
  if (a[0] != ai_magic[0] || a[1] != ai_magic[1] || a[2] != ai_magic[2] || a[3] != ai_magic[3])
  {
    throw std::runtime_error("AstroImageIO: not an internal astro image");
  }
  a = f.read(sizeof(uint32_t));
  uint32_t version = *(reinterpret_cast<uint32_t*>(a.data()));
  std::shared_ptr<FitsImage> img;
  switch (version)
  {
    case 0x0100:
      img = read_0x0100(&f,info);
      break;
    case 0x0200:
      img = read_0x0200(&f,info);
      break;
    default:
      throw std::runtime_error("AstroImageIO: unsupported version");
      break;
  }
  profiler.stop();
  logProfiler(img,"read");
  return {std::make_shared<FitsObject>(img,filename)};
}

bool AstroImageIO::write(QString /*filename*/, FitsObject* /*img*/)
{
  throw std::runtime_error("Writing to astro image format not supported.");
}



std::shared_ptr<FitsImage> AstroImageIO::read_0x0100(QFile *f, const QFileInfo& info)
{
  ImageMetadata metadata;
  /* image data */
  QByteArray a = f->read(sizeof(uint64_t));
  uint64_t msec = *(reinterpret_cast<uint64_t*>(a.data()));
  metadata.setObsDateTime(QDateTime::fromMSecsSinceEpoch(msec));
  a = f->read(sizeof(uint32_t));
  uint32_t w = *(reinterpret_cast<uint32_t*>(a.data()));
  a = f->read(sizeof(uint32_t));
  uint32_t h = *(reinterpret_cast<uint32_t*>(a.data()));
  a = f->read(sizeof(uint32_t));
  uint32_t bpp = *(reinterpret_cast<uint32_t*>(a.data()));
  if (bpp != 8) throw std::runtime_error("AstroImageIO: bpp!=8 is not supported");
  a = f->read(sizeof(double));
  metadata.setExposureTime(*(reinterpret_cast<double*>(a.data())));
  /* metadata table */
  a = f->read(sizeof(uint32_t));
  uint32_t rows = *(reinterpret_cast<uint32_t*>(a.data()));
  while (rows > 0)
  {
    QString key = f->read(KEY_LEN);
    QString value = f->read(VALUE_LEN);
    if (key == ai_key_object)
      metadata.setObject(value);
    else if (key == ai_key_observer)
      metadata.setObserver(value);
    else if (key == ai_key_instrument)
      metadata.setInstrument(value);
    else if (key == ai_key_telescope)
      metadata.setTelescope(value);
    --rows;
  }
  /* the image data */
  a = f->read(sizeof(uint32_t));
  uint32_t compressed = *(reinterpret_cast<uint32_t*>(a.data()));
  a = f->read(sizeof(uint32_t));
  uint32_t size = *(reinterpret_cast<uint32_t*>(a.data()));
  QByteArray d = f->read(size);
  if (compressed) d = qUncompress(d);
  auto img = std::make_shared<FitsImage>(info.baseName(),w,h,1);
  PixelIterator it = img->getPixelIterator();
  for (uint8_t v : std::as_const(d))
  {
    it[0] = v;
    ++it;
  }
  img->setMetadata(metadata);
  return img;
}

std::shared_ptr<FitsImage> AstroImageIO::read_0x0200(QFile *f, const QFileInfo& info)
{
  ImageMetadata metadata;
  /* image data */
  QByteArray a = f->read(sizeof(uint64_t));
  uint64_t msec = *(reinterpret_cast<uint64_t*>(a.data()));
  metadata.setObsDateTime(QDateTime::fromMSecsSinceEpoch(msec));
  a = f->read(sizeof(uint32_t));
  uint32_t w = *(reinterpret_cast<uint32_t*>(a.data()));
  a = f->read(sizeof(uint32_t));
  uint32_t h = *(reinterpret_cast<uint32_t*>(a.data()));
  a = f->read(sizeof(uint32_t));
  uint32_t bytesPerPixel = *(reinterpret_cast<uint32_t*>(a.data()));
  if (bytesPerPixel != 1 && bytesPerPixel != 2 && bytesPerPixel != 4) throw std::runtime_error("AstroImageIO: unsupported bytes per pixel count");
  a = f->read(sizeof(double));
  metadata.setExposureTime(*(reinterpret_cast<double*>(a.data())));
  /* metadata table */
  a = f->read(sizeof(uint32_t));
  uint32_t rows = *(reinterpret_cast<uint32_t*>(a.data()));
  while (rows > 0)
  {
    QString key = f->read(KEY_LEN);
    QString value = f->read(VALUE_LEN);
    if (key == ai_key_object)
      metadata.setObject(value);
    else if (key == ai_key_observer)
      metadata.setObserver(value);
    else if (key == ai_key_instrument)
      metadata.setInstrument(value);
    else if (key == ai_key_telescope)
      metadata.setTelescope(value);
    --rows;
  }
  /* the image data */
  a = f->read(sizeof(uint32_t));
  uint32_t compressed = *(reinterpret_cast<uint32_t*>(a.data()));
  a = f->read(sizeof(uint32_t));
  uint32_t size = *(reinterpret_cast<uint32_t*>(a.data()));
  QByteArray d = f->read(size);
  if (compressed) d = qUncompress(d);
  std::shared_ptr<FitsImage> img;
  if (bytesPerPixel == 1)
  {
    img = std::make_shared<FitsImage>(info.baseName(),w,h,1);
    PixelIterator it = img->getPixelIterator();
    for (uint8_t v : std::as_const(d))
    {
      it[0] = v;
      ++it;
    }
  }
  else if (bytesPerPixel == 2)
  {
    img = std::make_shared<FitsImage>(info.baseName(),w,h,1);
    uint16_t* v = reinterpret_cast<uint16_t*>(d.data());
    PixelIterator it = img->getPixelIterator();
    for (uint32_t i=0;i<w*h;i++)
    {
      it[0] = *v;
      ++v;
      ++it;
    }
  }
  else
  {
    img = std::make_shared<FitsImage>(info.baseName(),w,h,3);
    uint32_t* v = reinterpret_cast<uint32_t*>(d.data());
    PixelIterator it = img->getPixelIterator();
    for (uint32_t i=0;i<w*h;i++)
    {
      it[0] = (*v >> 16) & 0xFF;
      it[1] = (*v >> 8) & 0xFF;
      it[2] = *v & 0xFF;
      ++v;
      ++it;
    }
  }
  img->setMetadata(metadata);
  return img;
}
