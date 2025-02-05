/********************************************************************************
 *                                                                              *
 * FitsIP - factory for I/O handlers                                            *
 *                                                                              *
 * modified: 2025-02-05                                                         *
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

#ifndef IOFACTORY_H
#define IOFACTORY_H

#include "iohandler.h"
#include <memory>
#include <map>

class IOFactory: public QObject
{
  Q_OBJECT
public:
  ~IOFactory();

  bool isImage(QString filename);

  IOHandler* getHandler(QString filename);

  QString getReadFilters() const;

  QString getWriteFilters() const;

  static IOFactory* getInstance();

  static QString assertSuffix(const QString& fn, QString filter);

signals:
  void logProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

private:
  IOFactory();

  IOHandler* createHandler(QString filename);

  std::map<QString,IOHandler*> handlers;

  static IOFactory* instance;
};

#endif // IOFACTORY_H
