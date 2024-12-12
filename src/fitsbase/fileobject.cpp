/********************************************************************************
 *                                                                              *
 * FitsIP - file object containing the image and other data                     *
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

#include "fileobject.h"
#include "io/iofactory.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>

int32_t FileObject::idCounter = 0;

FileObject::FileObject(QString fn, std::shared_ptr<FitsImage> img):
  id(idCounter++),
  filename(fn),
  image(img)
{
//  QFileInfo info(filename);
  histogram = std::make_shared<Histogram>();
  histogram->build(image.get());
}

int32_t FileObject::getId() const
{
  return id;
}

QString FileObject::getFilename() const
{
  return filename;
}

std::shared_ptr<FitsImage> FileObject::getImage()
{
  return image;
}

std::shared_ptr<Histogram> FileObject::getHistogram()
{
  return histogram;
}

void FileObject::updateHistogram()
{
  histogram->build(image.get());
}

bool FileObject::save(QString fn)
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
    filename = fn;
  }
  catch (std::exception& ex)
  {
    if (!tmpfile.isEmpty()) QFile(tmpfile).rename(fn);
    throw ex;
  }
  if (!tmpfile.isEmpty()) QFile(tmpfile).remove();
  return true;
}

void FileObject::pushUndo()
{
  undostack.push(image);
}

void FileObject::popUndo()
{
  std::shared_ptr<FitsImage> img = undostack.pop();
  if (img)
  {
    image = img;
    updateHistogram();
  }
}

bool FileObject::isUndoAvailable() const
{
  return undostack.isUndoAvailable();
}

