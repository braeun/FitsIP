/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for image I/O handlers                           *
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

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "../profiling/simpleprofiler.h"
#include <QObject>
#include <QString>
#include <memory>

class FitsImage;
class QWidget;

class IOHandler: public QObject
{
  Q_OBJECT
public:
  IOHandler(QObject* parent=nullptr);
  virtual ~IOHandler();

  virtual std::vector<std::shared_ptr<FitsImage>> read(QString filename) = 0;

  virtual bool write(QString filename, std::shared_ptr<FitsImage> img) = 0;

signals:
  void logProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

protected:
  void logProfiler(const QString& image, const QString& msg="");

  void logProfiler(std::shared_ptr<FitsImage> image, const QString& msg="");

  SimpleProfiler profiler;

};

#endif // IOHANDLER_H
