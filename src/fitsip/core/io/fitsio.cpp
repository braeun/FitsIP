/********************************************************************************
 *                                                                              *
 * FitsIP - FITS image format reader and writer                                 *
 *                                                                              *
 * modified: 2025-10-24                                                         *
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

std::vector<std::shared_ptr<FitsImage>> FitsIO::read(QString filename)
{
  QFileInfo info(filename);
  try
  {
    profiler.start();
    CCfits::FITS fits(filename.toStdString());
    ImageMetadata metadata;
    fits.pHDU().readAllKeys();
    const std::map<std::string,CCfits::Keyword*>& keywords = fits.pHDU().keyWord();
    std::string v;
    //    QString date = "";
    //    QString time = "";
    for (const auto& entry : keywords)
    {
      if (entry.second->keytype() == CCfits::Tlogical)
      {
        bool flag;
        flag = entry.second->value(flag);
        metadata.addEntry(QString::fromStdString(entry.first),flag?"T":"F",QString::fromStdString(entry.second->comment()));
      }
      else
      {
        metadata.addEntry(QString::fromStdString(entry.first),QString::fromStdString(entry.second->value(v)),QString::fromStdString(entry.second->comment()));
      }
    }
    QString history = QString::fromStdString(fits.pHDU().history());
    metadata.setHistory(history.split("\n"));

    std::vector<std::shared_ptr<FitsImage>> list;
    if (fits.pHDU().axes() >= 2)
    {
      auto img = load(&fits.pHDU(),info.baseName(),metadata);
      if (img) list.push_back(img);
    }
    if (fits.pHDU().extend())
    {
      for (size_t i=1;i<=fits.extension().size();++i)
      {
        std::string v;
        fits.extension(i).readKey("XTENSION",v);
        if (v.find("IMAGE") == std::string::npos) continue;
        auto img = load(&fits.extension(i),info.baseName(),metadata);
        if (img) list.push_back(img);
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
    logProfiler(info.baseName(),"read");
    return list;
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

std::shared_ptr<FitsImage> FitsIO::load(CCfits::HDU* hdu, QString basename, const ImageMetadata& basedata)
{
  if (hdu->axes() < 2) return {};
  QString name = basename;
  if (dynamic_cast<CCfits::ExtHDU*>(hdu))
  {
    name += " [" + QString::fromStdString(dynamic_cast<CCfits::ExtHDU*>(hdu)->name()) + "]";
  }
  long w = hdu->axis(0);
  long h = hdu->axis(1);
  long depth = 1;
  if (hdu->axes() > 2) depth = hdu->axis(2);
  auto img = std::make_shared<FitsImage>(name,static_cast<int>(w),static_cast<int>(h),static_cast<int>(depth));
  int64_t first = 1;
  std::valarray<ValueType> a;
  for (long i=0;i<depth;i++)
  {
    if (dynamic_cast<CCfits::PHDU*>(hdu))
      dynamic_cast<CCfits::PHDU*>(hdu)->read(a,first,w*h);
    else if (dynamic_cast<CCfits::ExtHDU*>(hdu))
      dynamic_cast<CCfits::ExtHDU*>(hdu)->read(a,first,w*h);
    else
      return std::shared_ptr<FitsImage>();
    img->getLayer(i)->setData(a);
    first += w * h;
  }

  if (dynamic_cast<CCfits::ExtHDU*>(hdu))
  {
    ImageMetadata metadata;
    hdu->readAllKeys();
    const std::map<std::string,CCfits::Keyword*>& keywords = hdu->keyWord();
    std::string v;
    if (keywords.find("INHERIT") != keywords.end())
    {
      bool flag;
      if (keywords.at("INHERIT")->value(flag))
      {
        metadata = basedata;
      }
    }
    for (const auto& entry : keywords)
    {
      if (entry.second->keytype() == CCfits::Tlogical)
      {
        bool flag;
        flag = entry.second->value(flag);
        metadata.addEntry(QString::fromStdString(entry.first),flag?"T":"F",QString::fromStdString(entry.second->comment()));
      }
      else
      {
        metadata.addEntry(QString::fromStdString(entry.first),QString::fromStdString(entry.second->value(v)),QString::fromStdString(entry.second->comment()));
      }
    }
    QString history = QString::fromStdString(hdu->history());
    metadata.setHistory(history.split("\n"));
    img->setMetadata(metadata);
    /* special handling for starlight xpress which stores unsigned 16bit */
    if (metadata.getInstrument().toLower().contains("starlight xpress") && hdu->bitpix() == 16)
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
  }
  return img;
}
