/********************************************************************************
 *                                                                              *
 * FitsIP - plugin factory                                                      *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

class QPluginLoader;

class PluginFactory: public QObject
{
  Q_OBJECT
public:
  PluginFactory();
  virtual ~PluginFactory();

#ifdef USE_PYTHON
  void setBinding(py::module_* m);
#endif

  QObjectList getPlugins();

  Plugin* loadPlugin(QString filename);

  static PluginFactory* instance();

signals:
  void logOperation(QString image, QString op);

  void logProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

private:

  std::vector<std::shared_ptr<QPluginLoader>> plugins;
#ifdef USE_PYTHON
  py::module_* pymod;
#endif

  static std::unique_ptr<PluginFactory> factory;
};

#endif // PLUGINFACTORY_H
