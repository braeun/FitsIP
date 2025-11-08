/********************************************************************************
 *                                                                              *
 * FitsIP - fits object containing the image and other data                     *
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

#include "fitsobject.h"
#include "io/iofactory.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>

int FitsObject::idCounter = 0;

FitsObject::FitsObject(const FitsImage& img, const QString&  fn):
  id(idCounter++),
  filename(fn),
  image(img)
{
  // pixelList = std::make_unique<PixelList>();
  // starList = std::make_unique<StarList>();
  // annotations = std::make_unique<Annotations>();
//  QFileInfo info(filename);
//  histogram.build(image.get());
}

FitsObject::FitsObject(const FitsImage& img, const std::string&  fn):
 id(idCounter++),
 filename(QString::fromStdString(fn)),
 image(img)
{
  // pixelList = std::make_unique<PixelList>();
  // starList = std::make_unique<StarList>();
//  QFileInfo info(filename);
//  histogram.build(image.get());
}

FitsObject::~FitsObject()
{

}

int32_t FitsObject::getId() const
{
  return id;
}

QString FitsObject::getName() const
{
  return image.getName();
}

QString  FitsObject::getFilename() const
{
  return filename;
}

const FitsImage& FitsObject::getImage() const
{
  return image;
}

FitsImage& FitsObject::getImage()
{
  return image;
}

void FitsObject::setImage(const FitsImage& img)
{
  image = img;
}

QRect FitsObject::getAOI() const
{
  return aoi;
}

void FitsObject::setAOI(const QRect& r)
{
  aoi = r;
}

const Histogram& FitsObject::getHistogram(bool update)
{
  if (histogram.isEmpty() || update)
  {
    histogram.build(image);
  }
  return histogram;
}

void FitsObject::updateHistogram()
{
  histogram.build(image);
}

void FitsObject::setXProfile(const Profile& p)
{
  xprofile = p;
}

const Profile& FitsObject::getXProfile() const
{
  return xprofile;
}

void FitsObject::setYProfile(const Profile& p)
{
  yprofile = p;
}

const Profile& FitsObject::getYProfile() const
{
  return yprofile;
}

PixelList* FitsObject::getPixelList()
{
  return &pixelList;
}

const PixelList* FitsObject::getPixelList() const
{
  return &pixelList;
}

StarList* FitsObject::getStarList()
{
  return &starList;
}

const StarList* FitsObject::getStarList() const
{
  return &starList;
}

Annotations* FitsObject::getAnnotations()
{
  return &annotations;
}

const Annotations* FitsObject::getAnnotations() const
{
  return &annotations;
}

void FitsObject::addXYData(const XYData& data)
{
  xydata.push_back(data);
}

void FitsObject::addXYData(const std::vector<XYData>& data)
{
  xydata.insert(xydata.begin(),data.begin(),data.end());
}

void FitsObject::resetXYData()
{
  xydata.clear();
}

const std::vector<XYData>& FitsObject::getXYData()
{
  return xydata;
}

bool FitsObject::save(const QString& fn)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(fn);
  if (!handler) return false;
  QString tmpfile = "";
  QFile file(fn);
  if (file.exists())
  {
    tmpfile = fn + "~";
    file.rename(tmpfile);
  }
  try
  {
    handler->write(fn,*this);
    this->filename = fn;
  }
  catch (std::exception& ex)
  {
    qCritical() << ex.what();
    if (!tmpfile.isEmpty()) QFile(tmpfile).rename(fn);
    throw ex;
  }
  if (!tmpfile.isEmpty()) QFile(tmpfile).remove();
  return true;
}

bool FitsObject::save(const std::string& fn)
{
  return save(QString::fromStdString(fn));
}

void FitsObject::pushUndo()
{
  undostack.push(image);
}

void FitsObject::popUndo()
{
  FitsImage img = undostack.pop();
  if (img)
  {
    image = img;
    updateHistogram();
  }
}

bool FitsObject::isUndoAvailable() const
{
  return undostack.isUndoAvailable();
}

std::shared_ptr<FitsObject> FitsObject::copy(const std::string& filename) const
{
  auto obj = std::make_shared<FitsObject>(image,filename);
  obj->histogram = histogram;
  obj->xprofile = xprofile;
  obj->yprofile = yprofile;
  return obj;
}
