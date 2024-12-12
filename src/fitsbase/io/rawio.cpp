/********************************************************************************
 *                                                                              *
 * FitsIP - DSLR raw image format reader                                        *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#include "rawio.h"
#include "../fitsimage.h"
#include <libraw/libraw.h>
#include <QFileInfo>

const char* RawIO::FILENAME_FILTER = "Canon Raw Data (*.crw);;Other Raw Data (*)";

RawIO::RawIO()
{
}

RawIO::~RawIO()
{
}

bool RawIO::handlesFile(const QString &filename)
{
  LibRaw ip;
  int32_t err = ip.open_file(filename.toStdString().c_str());
  return err == LIBRAW_SUCCESS;
}

std::shared_ptr<FitsImage> RawIO::read(QString filename)
{
  QFileInfo info(filename);
  profiler.start();
  LibRaw ip;
  int32_t err = ip.open_file(filename.toStdString().c_str());
  if (err != LIBRAW_SUCCESS)
  {
    if (err == LIBRAW_FILE_UNSUPPORTED) throw std::runtime_error("Unsupported raw image file");
    throw std::runtime_error("Failed to load image");
  }
  err = ip.unpack();
  if (err != LIBRAW_SUCCESS) throw std::runtime_error("Failed to load image");
  err = ip.dcraw_process();
  if (err != LIBRAW_SUCCESS) throw std::runtime_error("Failed to load image");
  libraw_processed_image_t *image = ip.dcraw_make_mem_image(&err);
  if (image)
  {
    if (image->type != LIBRAW_IMAGE_BITMAP) throw std::runtime_error("Failed to load image");
    if (image->colors != 3 && image->colors != 1) throw std::runtime_error("Only monochrome and 3-color images supported");
    if (image->bits != 8) throw std::runtime_error("Only 8 bpp images supported");
    auto img = std::make_shared<FitsImage>(info.baseName(),image->width,image->height,image->colors);
    PixelIterator it = img->getPixelIterator();
    if (image->colors == 1)
    {
      uint8_t* ptr = image->data;
      for (uint32_t i=0;i<image->data_size;i++)
      {
        it[0] = *ptr;
        ++it;
        ++ptr;
      }
    }
    else
    {
      uint8_t* ptr = image->data;
      for (uint32_t i=0;i<image->data_size/3;i++)
      {
        it[0] = *ptr++;
        it[1] = *ptr++;
        it[2] = *ptr++;
        ++it;
      }
    }
    LibRaw::dcraw_clear_mem(image);
    ImageMetadata meta = img->getMetadata();
    meta.exposure = ip.imgdata.other.shutter;
    meta.observer = ip.imgdata.other.artist;
    meta.instrument = ip.imgdata.idata.model;
    meta.date = QDateTime::fromTime_t(ip.imgdata.other.timestamp);
    img->setMetadata(meta);
    profiler.stop();
    logProfiler(img,"read");
    return img;
  }
  else
    throw std::runtime_error("Failed to load image");
}

bool RawIO::write(QString /*filename*/, std::shared_ptr<FitsImage> /*img*/)
{
  throw std::runtime_error("Writing to raw format not supported.");
}
