/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for image I/O handlers                           *
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

#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "../profiling/simpleprofiler.h"
#include <QObject>
#include <QString>
#include <memory>

class FitsImage;
class FitsObject;
class QWidget;

class IOHandler: public QObject
{
  Q_OBJECT
public:
  IOHandler(QObject* parent=nullptr);
  virtual ~IOHandler();

  virtual std::vector<std::shared_ptr<FitsObject>> read(QString filename) = 0;

  virtual bool write(QString filename, const FitsObject& obj) = 0;

  /**
   * @brief Convenience method!
   *
   * This method make a temporary copy of the image.
   * @param filename
   * @param img
   * @return
   */
  virtual bool write(QString filename, const FitsImage& img);

signals:
  void logProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

protected:
  void logProfiler(const QString& image, const QString& msg="");

  void logProfiler(const FitsImage& image, const QString& msg="");

  SimpleProfiler profiler;

};

#endif // IOHANDLER_H
