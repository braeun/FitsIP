﻿/********************************************************************************
 *                                                                              *
 * FitsIP - fits object containing the image and other data                     *
 *                                                                              *
 * modified: 2025-02-01                                                         *
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

FitsObject::FitsObject(std::shared_ptr<FitsImage> img, const QString&  fn):
  id(idCounter++),
  filename(fn),
  image(img)
{
//  QFileInfo info(filename);
//  histogram.build(image.get());
}

FitsObject::FitsObject(std::shared_ptr<FitsImage> img, const std::string&  fn):
 id(idCounter++),
 filename(QString::fromStdString(fn)),
 image(img)
{
//  QFileInfo info(filename);
//  histogram.build(image.get());
}

int32_t FitsObject::getId() const
{
  return id;
}

QString FitsObject::getName() const
{
  return image->getName();
}

QString  FitsObject::getFilename() const
{
  return filename;
}

std::shared_ptr<FitsImage> FitsObject::getImage() const
{
  return image;
}

void FitsObject::setImage(const std::shared_ptr<FitsImage>& img)
{
  image = img;
}

const Histogram& FitsObject::getHistogram(bool update)
{
  if (histogram.isEmpty() || update)
  {
    histogram.build(image.get());
  }
  return histogram;
}

void FitsObject::updateHistogram()
{
  histogram.build(image.get());
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
    handler->write(fn,image);
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
  std::shared_ptr<FitsImage> img = undostack.pop();
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

std::shared_ptr<FitsObject> FitsObject::clone(const std::string& filename) const
{
  auto img = std::make_shared<FitsImage>(*image.get());
  auto obj = std::make_shared<FitsObject>(img,filename);
  obj->histogram = histogram;
  obj->xprofile = xprofile;
  obj->yprofile = yprofile;
  return obj;
}
