/********************************************************************************
 *                                                                              *
 * FitsIP - FITS image format reader and writer                                 *
 *                                                                              *
 * modified: 2025-03-14                                                         *
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

#include "fitsio.h"
#include "../fitstypes.h"
#include "../fitsimage.h"
#include "../settings.h"
#include <CCfits/CCfits>
#include <CCfits/FITSUtil.h>
#include <QDate>
#include <QFileInfo>
#include <QSettings>
#include <QTime>
#include <QDebug>

const char* FitsIO::FILENAME_FILTER = "FITS Image (*.fts *.fit *.fits *.fts.gz *.fit.gz *.fits.gz)";

FitsIO::FitsIO()
{
  profiler = SimpleProfiler("FitsIO");
}

FitsIO::~FitsIO()
{
}

std::shared_ptr<FitsImage> FitsIO::read(QString filename)
{
  QFileInfo info(filename);
  try
  {
    profiler.start();
    CCfits::FITS fits(filename.toStdString());
    long w = fits.pHDU().axis(0);
    long h = fits.pHDU().axis(1);
    long depth = 1;
    if (fits.pHDU().axes() > 2) depth = fits.pHDU().axis(2);
    auto img = std::make_shared<FitsImage>(info.baseName(),static_cast<int>(w),static_cast<int>(h),static_cast<int>(depth));
    int64_t first = 1;
    std::valarray<ValueType> a;
    for (long i=0;i<depth;i++)
    {
      fits.pHDU().read(a,first,w*h);
      img->getLayer(i)->setData(a);
      first += w * h;
    }

    ImageMetadata metadata;
    fits.pHDU().readAllKeys();
    const std::map<std::string,CCfits::Keyword*>& keywords = fits.pHDU().keyWord();
    std::string v;
//    QString date = "";
//    QString time = "";
    for (const auto& entry : keywords)
    {
      metadata.addEntry(QString::fromStdString(entry.first),QString::fromStdString(entry.second->value(v)));
//      qDebug() << entry.first.c_str() << ": " << entry.second->value(v).c_str();
//      if (entry.first == "OBJECT") metadata.object = QString::fromStdString(entry.second->value(v));
//      if (entry.first == "INSTRUME") metadata.instrument = QString::fromStdString(entry.second->value(v));
//      if (entry.first == "TELESCOP") metadata.telescope = QString::fromStdString(entry.second->value(v));
//      if (entry.first == "OBSERVER") metadata.observer = QString::fromStdString(entry.second->value(v));
//      if (entry.first == "EXPTIME") metadata.exposure = QString::fromStdString(entry.second->value(v)).toDouble();
//      if (entry.first == "DATE-OBS") date = QString::fromStdString(entry.second->value(v));
//      if (entry.first == "TIME-OBS") time = QString::fromStdString(entry.second->value(v));
    }
//    if (!date.isEmpty() && !time.isEmpty())
//    {
//      QDate d = QDate::fromString(date,Qt::ISODate);
//      QTime t = QTime::fromString(time,Qt::ISODate);
//      metadata.date = QDateTime(d,t,Qt::UTC);
//    }
    QString history = QString::fromStdString(fits.pHDU().history());
    metadata.setHistory(history.split("\n"));
//    if (!metadata.history.empty() && metadata.history.back().trimmed().isEmpty())
//    {
//      metadata.history.pop_back();
//    }
    img->setMetadata(metadata);
    /* special handling for starlight xpress which stores unsigned 16bit */
    if (metadata.getInstrument().toLower().contains("starlight xpress") && fits.pHDU().bitpix() == 16)
    {
      PixelIterator it = img->getPixelIterator();
      while (true)
      {
        for (long i=0;i<depth;i++)
        {
          if (it[i] < 0) it[i] += 0x10000;
        }
        if (!it.hasNext()) break;
        ++it;
      }
    }

//    qDebug() << fits.pHDU().history().c_str();
//    qDebug() << fits.pHDU().comment().c_str();

//    qDebug() << fits.pHDU().simple();
//    qDebug() << fits.pHDU().bitpix();
//    qDebug() << fits.pHDU().zero();
//    qDebug() << fits.pHDU().scale();
//    qDebug() << fits.pHDU().axes();
//    for (int64_t i=0;i<fits.pHDU().axes();i++)
//    {
//      qDebug() << fits.pHDU().axis(i);
//    }
    profiler.stop();
    logProfiler(img,"read");
    return img;
  }
  catch (CCfits::FitsException& ex)
  {
    throw std::runtime_error(ex.message());
  }
}

bool FitsIO::write(QString filename, std::shared_ptr<FitsImage> img)
{
  Settings settings;
  int format = settings.getFitsImageFormat();
  CCfits::FITS* fits = nullptr;
  try
  {
    profiler.start();
    int64_t axes[]{img->getWidth(),img->getHeight(),img->getDepth()};
    switch (format)
    {
      case 0:
      default:
        fits = new CCfits::FITS("!"+filename.toStdString(),DOUBLE_IMG,img->getDepth()==1?2:3,axes);
        break;
      case 1:
        fits = new CCfits::FITS("!"+filename.toStdString(),FLOAT_IMG,img->getDepth()==1?2:3,axes);
        break;
    }
    int64_t first = 1;
    for (int i=0;i<img->getDepth();i++)
    {
      std::shared_ptr<Layer> layer = img->getLayer(i);
      std::valarray<ValueType> a(layer->getData(),static_cast<uint64_t>(axes[0]*axes[1]));
      fits->pHDU().write(first,axes[0]*axes[1],a);
      first += axes[0] * axes[1];
    }
    const ImageMetadata& metadata = img->getMetadata();
    for (const auto& entry : metadata.getEntries())
    {
      fits->pHDU().addKey(entry.first.toStdString(),entry.second.value.toStdString(),entry.second.comment.toStdString());
    }
//    if (!metadata.object.isEmpty()) fits->pHDU().addKey("OBJECT",metadata.object.toStdString().c_str(),"");
//    if (!metadata.instrument.isEmpty()) fits->pHDU().addKey("INSTRUME",metadata.instrument.toStdString().c_str(),"");
//    if (!metadata.telescope.isEmpty()) fits->pHDU().addKey("TELESCOP",metadata.telescope.toStdString().c_str(),"");
//    if (!metadata.observer.isEmpty()) fits->pHDU().addKey("OBSERVER",metadata.observer.toStdString().c_str(),"");
//    if (metadata.exposure > 0) fits->pHDU().addKey("EXPTIME",metadata.exposure,"");
//    if (!metadata.date.isNull())
//    {
//      fits->pHDU().addKey("DATE-OBS",metadata.date.date().toString(Qt::ISODate).toStdString().c_str(),"");
//      fits->pHDU().addKey("TIME-OBS",metadata.date.time().toString(Qt::ISODate).toStdString().c_str(),"");
//    }
    if (!metadata.getHistory().isEmpty())
    {
      fits->pHDU().writeHistory(metadata.getHistory().join("\n").toStdString());
    }
    delete fits;
    profiler.stop();
    logProfiler(img,"write");
  }
  catch (std::exception& ex)
  {
    if (fits) delete fits;
    auto p = std::current_exception();
    if (p) std::rethrow_exception(p);
  }
  return true;
}

