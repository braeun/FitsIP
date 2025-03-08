/********************************************************************************
 *                                                                              *
 * FitsIP - plugin factory                                                      *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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

#include "pluginfactory.h"
#include <QtPlugin>
#include <QPluginLoader>
#include <QDebug>
#include <utility>

PluginFactory::PluginFactory()
{
  for (const QObject* o : QPluginLoader::staticInstances())
  {
    const OpPlugin* p = dynamic_cast<const OpPlugin*>(o);
    if (p)
    {
      connect(p,&OpPlugin::logOperation,this,&PluginFactory::logOperation);
      connect(p,&OpPlugin::logProfilerResult,this,&PluginFactory::logProfilerResult);
    }
  }
}

PluginFactory::~PluginFactory()
{
}

#ifdef USE_PYTHON
void PluginFactory::setPythonBinding(void* m)
{
  pymod = m;
  for (const QObject* o : QPluginLoader::staticInstances())
  {
    const OpPlugin* p = dynamic_cast<const OpPlugin*>(o);
    if (p)
    {
      p->bindPython(m);
    }
  }
  for (std::shared_ptr<QPluginLoader> plugin : plugins)
  {
    Plugin* p = dynamic_cast<Plugin*>(plugin->instance());
    if (p)
    {
      const OpPlugin* op = dynamic_cast<const OpPlugin*>(p);
      if (op)
      {
        op->bindPython(m);
      }
    }
  }
}
#endif

QObjectList PluginFactory::getPlugins()
{
  QObjectList list = QPluginLoader::staticInstances();
  return list;
}

Plugin* PluginFactory::loadPlugin(QString filename)
{
  for (std::shared_ptr<QPluginLoader> plugin : plugins)
  {
    if (plugin->fileName() == filename) return dynamic_cast<Plugin*>(plugin->instance());
  }
  std::shared_ptr<QPluginLoader> plugin = std::make_shared<QPluginLoader>(filename);
  bool ret = plugin->load();
  if (!ret)
  {
    qDebug() << "plugin load failed!";
    return nullptr;
  }
  Plugin* p = dynamic_cast<Plugin*>(plugin->instance());
  if (p)
  {
    plugins.push_back(plugin);
    OpPlugin* op = dynamic_cast<OpPlugin*>(p);
    if (op)
    {
#ifdef USE_PYTHON
      op->bindPython(pymod);
#endif
      connect(op,&OpPlugin::logOperation,this,&PluginFactory::logOperation);
      connect(op,&OpPlugin::logProfilerResult,this,&PluginFactory::logProfilerResult);
    }
  }
  return p;
}

