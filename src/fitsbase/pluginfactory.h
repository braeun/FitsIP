/********************************************************************************
 *                                                                              *
 * FitsIP - plugin factory                                                      *
 *                                                                              *
 * modified: 2025-02-20                                                         *
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

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include "opplugin.h"
#include <QObject>
#include <memory>
#include <vector>

class ImageCollection;
class QPluginLoader;

class PluginFactory: public QObject
{
  Q_OBJECT
public:
  PluginFactory();
  virtual ~PluginFactory();

#ifdef USE_PYTHON
  void setPythonBinding(void* m);
#endif

  QObjectList getPlugins();

  Plugin* loadPlugin(QString filename);

  void setImageCollection(ImageCollection* col);

signals:
  void logOperation(QString image, QString op);

  void logProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

private:

  std::vector<std::shared_ptr<QPluginLoader>> plugins;
  ImageCollection* imageCollection;
#ifdef USE_PYTHON
  void* pymod;
#endif
};

#endif // PLUGINFACTORY_H
