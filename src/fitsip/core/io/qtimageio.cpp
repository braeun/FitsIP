/********************************************************************************
 *                                                                              *
 * FitsIP - reader and writer for image formats handled by Qt                   *
 *                                                                              *
 * modified: 2025-11-06                                                         *
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

#include "qtimageio.h"
#include "../fitsimage.h"
#include "../fitsobject.h"
#include "../histogram.h"
#include "../settings.h"
#include <QFileInfo>
#include <QImage>
#include <QDebug>
#include <QTextStream>
#include <stdexcept>

const char* QtImageIO::FILENAME_FILTER = "Portable Network Graphics (*.png);;JPEG (*.jpg *.jpeg);;Bitmaps (*.bmp)";

QtImageIO::QtImageIO()
{
  profiler = SimpleProfiler("QtImageIO");
}

QtImageIO::~QtImageIO()
{
}

std::vector<std::shared_ptr<FitsObject>> QtImageIO::read(QString filename)
{
  profiler.start();
  QImage i(filename);
  if (i.isNull()) throw std::runtime_error("Failed to load image: "+filename.toStdString());
  QFileInfo info(filename);
  FitsImage img;
  if (i.depth() == 8)
  {
    img = FitsImage(info.baseName(),i.width(),i.height(),1);
    PixelIterator pixel = img.getPixelIterator();
    for (int y=0;y<i.height();y++)
    {
      const uchar* ptr = i.scanLine(y);
      for (int x=0;x<i.width();x++)
      {
        pixel[0] = *ptr;
        ++ptr;
        ++pixel;
      }
    }
  }
  else
  {
    img = FitsImage(info.baseName(),i.width(),i.height(),3);
    PixelIterator pixel = img.getPixelIterator();
    for (int y=0;y<i.height();y++)
    {
      const QRgb* rgb = reinterpret_cast<QRgb*>(i.scanLine(y));
      for (int x=0;x<i.width();x++)
      {
        RGBValue v(*rgb);
        pixel[0] = v.red();
        pixel[1] = v.green();
        pixel[2] = v.blue();
        ++rgb;
        ++pixel;
      }
    }
  }
  ImageMetadata data = img.getMetadata();
  QStringList keys = i.textKeys();
  for (const QString& key : keys)
  {
    QString value =i.text(key);
    if (key == "date")
      data.setObsDateTime(QDateTime::fromString(value,Qt::ISODateWithMs));
    else if (key == "object")
      data.addEntry(ImageMetadata::keyObject,value);
    else if (key == "observer")
      data.addEntry(ImageMetadata::keyObserver,value);
    else if (key == "instrument")
      data.addEntry(ImageMetadata::keyInstrument,value);
    else if (key == "telescope")
      data.addEntry(ImageMetadata::keyTelescope,value);
    else if (key == "exposure")
      data.addEntry(ImageMetadata::keyExpTime,value);
    qInfo() << key << i.text(key);
  }
  readMetadata(filename,&data);
#ifdef HAVE_EXIV2
  readExif(filename,&data);
#endif
  img.setMetadata(data);
  profiler.stop();
  logProfiler(img,"read");
  return {std::make_shared<FitsObject>(img,filename)};
}

bool QtImageIO::write(QString filename, const FitsObject& obj)
{
  profiler.start();
  const FitsImage& img = obj.getImage();
  Histogram h;
  h.build(img);
  QImage i = img.toQImage(h.getMin(),h.getMax(),FitsImage::LINEAR);
  if (!i.save(filename)) return false;
  if (Settings().isWriteMetadataFile()) writeMetadata(filename,img.getMetadata());
#ifdef HAVE_EXIV2
  writeExif(filename,img);
#endif
  profiler.stop();
  logProfiler(img,"write");
  return true;
}




void QtImageIO::readMetadata(QString filename, ImageMetadata* data)
{
  QFileInfo info(filename);
  QString datafile = info.absolutePath() + "/" + info.baseName() + ".txt";
  QFile file(datafile);
  if (file.exists() && file.open(QIODevice::ReadOnly|QIODevice::Text))
  {
    while (!file.atEnd())
    {
      QByteArray line = file.readLine();
      QStringList tokens = QString(line).split(":");
      if (tokens.size() >= 2)
      {
        QString id = tokens[0].trimmed();
        tokens.removeFirst();
        QString value = tokens.join(":").trimmed();
        if (id == "date")
          data->setObsDateTime(QDateTime::fromString(value,Qt::ISODateWithMs));
        else if (id == "object" && !data->hasEntry(ImageMetadata::keyObject))
          data->addEntry(ImageMetadata::keyObject,value);
        else if (id == "observer" && !data->hasEntry(ImageMetadata::keyObserver))
          data->addEntry(ImageMetadata::keyObserver,value);
        else if (id == "instrument" && !data->hasEntry(ImageMetadata::keyInstrument))
          data->addEntry(ImageMetadata::keyInstrument,value);
        else if (id == "telescope" && !data->hasEntry(ImageMetadata::keyTelescope))
          data->addEntry(ImageMetadata::keyTelescope,value);
        else if (id == "exposure" && !data->hasEntry(ImageMetadata::keyExpTime))
        {
          double v = value.toDouble();
          if (v < 1.0E-6) v = 0;
          data->addEntry(ImageMetadata::keyExpTime,QString::number(v));
        }
      }
    }
  }
}

void QtImageIO::writeMetadata(QString filename, const ImageMetadata& data)
{
  QFileInfo info(filename);
  QString datafile = info.absolutePath() + "/" + info.baseName() + ".txt";
  QFile file(datafile);
  if (file.open(QIODevice::WriteOnly|QIODevice::Text))
  {
    QTextStream os(&file);
    os << "date: " << data.getObsDateTime().toString(Qt::ISODateWithMs) << Qt::endl;
    os << "object: " << data.getValue(ImageMetadata::keyObject) << Qt::endl;
    os << "observer: " << data.getValue(ImageMetadata::keyObserver) << Qt::endl;
    os << "instrument: " << data.getValue(ImageMetadata::keyInstrument) << Qt::endl;
    os << "telescope: " << data.getValue(ImageMetadata::keyTelescope) << Qt::endl;
    os << "exposure: " << data.getValue(ImageMetadata::keyExpTime) << Qt::endl;
  }
}


#ifdef HAVE_EXIV2
#include <exiv2/exiv2.hpp>

void QtImageIO::readExif(QString filename, ImageMetadata* data)
{
  /* cannot handle files inside the Qt resource system */
  if (filename.startsWith(":")) return;
  auto image = Exiv2::ImageFactory::open(filename.toStdString());
  if (image.get() == nullptr) return;
  image->readMetadata();
  const Exiv2::ExifData &exifData = image->exifData();
  if (exifData.empty()) return;
  Exiv2::ExifData::const_iterator pos;
//  pos = exifData.findKey(Exiv2::ExifKey("Exif.Image.Orientation"));
//  if (pos != exifData.end())
//  {
//    switch (pos->value().toLong())
//    {
//      case 3:
//        orientation = 180;
//        break;
//      case 6:
//        orientation = 90;
//        break;
//      case 8:
//        orientation = 270;
//        break;
//    }
//  }
  pos = exifData.findKey(Exiv2::ExifKey("Exif.Image.DateTime"));
  if (pos != exifData.end())
  {
    std::cout << pos->value().toString() << std::endl;
    QStringList list = QString::fromStdString(pos->value().toString()).split(" ");
    if (!list.isEmpty())
    {
      if (list.size() > 1)
      {
        QDate date = QDate::fromString(list[0].replace(":","-"),Qt::ISODate);
        QTime time = QTime::fromString(list[1]);
        data->setObsDateTime(QDateTime(date,time));
      }
      else
      {
        QDateTime date = QDateTime::fromString(QString::fromStdString(pos->value().toString()),Qt::ISODate);
        data->setObsDateTime(date);
      }
      std::cout << data->getObsDateTime().toString().toStdString() << std::endl;
    }
  }
//  pos = exifData.findKey(Exiv2::ExifKey("Exif.Image.Make"));
//  if (pos != exifData.end()) addTag(MAKE,QString::fromStdString(pos->value().toString()),"Make");
  pos = exifData.findKey(Exiv2::ExifKey("Exif.Image.Model"));
  if (pos != exifData.end()) data->addEntry(ImageMetadata::keyInstrument,QString::fromStdString(pos->value().toString()));
  pos = exifData.findKey(Exiv2::ExifKey("Exif.Photo.CameraOwnerName"));
  if (pos != exifData.end())
    data->addEntry(ImageMetadata::keyObserver,QString::fromStdString(pos->value().toString()));
  else
  {
    pos = exifData.findKey(Exiv2::ExifKey("Exif.Canon.OwnerName"));
    if (pos != exifData.end()) data->addEntry(ImageMetadata::keyObserver,QString::fromStdString(pos->value().toString()));
  }
  pos = exifData.findKey(Exiv2::ExifKey("Exif.Photo.ExposureTime"));
  if (pos != exifData.end()) data->addEntry(ImageMetadata::keyExpTime,QString::number(pos->value().toFloat()));
//  pos = exifData.findKey(Exiv2::ExifKey("Exif.Image.ISOSpeedRatings"));
//  if (pos != exifData.end()) addTag(ISO,QString::fromStdString(pos->value().toString()),"ISOSpeedRatings");
  image->clearMetadata();
  image.reset();
}

void QtImageIO::writeExif(QString filename, const FitsImage& img)
{
  try {
    auto image = Exiv2::ImageFactory::open(filename.toStdString());
    image->readMetadata();
    Exiv2::ExifData& exifData = image->exifData();
    Exiv2::AsciiValue sv(img.getMetadata().getValue(ImageMetadata::keyObserver).toStdString());
    exifData.add(Exiv2::ExifKey("Exif.Photo.CameraOwnerName"),&sv);
    sv = Exiv2::AsciiValue(img.getMetadata().getValue(ImageMetadata::keyInstrument).toStdString());
    exifData.add(Exiv2::ExifKey("Exif.Image.Model"),&sv);
    Exiv2::FloatValue fv(img.getMetadata().getValue(ImageMetadata::keyExpTime).toFloat());
    exifData.add(Exiv2::ExifKey("Exif.Photo.ExposureTime"),&fv);
  //  image->setExifData(exifData);
    image->writeMetadata();
  } catch (...) {
  }
}

#endif


