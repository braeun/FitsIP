/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for image I/O handlers                           *
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

#include "iohandler.h"
#include "../fitsimage.h"
#include "../fitsobject.h"
#include <QDebug>

IOHandler::IOHandler(QObject* parent):QObject(parent)
{
}

IOHandler::~IOHandler()
{
}

bool IOHandler::write(QString filename, const FitsImage& img)
{
  FitsObject obj(img,filename);
  return write(filename,obj);
}

void IOHandler::logProfiler(const QString& image, const QString& msg)
{
  if (profiler.getDuration() > 0)
  {
    QString name = QString::fromStdString(profiler.getName());
    emit logProfilerResult(name,image,0,0,profiler.getDuration(),msg);
  }
}

void IOHandler::logProfiler(const FitsImage& image, const QString& msg)
{
  if (profiler.getDuration() > 0)
  {
    QString name = QString::fromStdString(profiler.getName());
    emit logProfilerResult(name,image.getName(),image.getWidth(),image.getHeight(),profiler.getDuration(),msg);
    //    qInfo() << profiler.toString().c_str();
  }
}


