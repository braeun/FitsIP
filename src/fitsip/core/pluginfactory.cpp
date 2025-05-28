/********************************************************************************
 *                                                                              *
 * FitsIP - plugin factory                                                      *
 *                                                                              *
 * modified: 2025-05-24                                                         *
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

#include "opplugin.h"
#include "pluginfactory.h"
#include "opplugincollection.h"
#include <fitsip/coreplugins/coreplugincollection.h>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QtPlugin>
#include <QPluginLoader>
#include <QDebug>
#include <utility>
#include <dlfcn.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const char* pathList = TOSTRING(PLUGIN_DIR) ";/usr/lib/fits;/usr/local/lib/fits";
//const char* pathList = "/usr/local/lib";

PluginFactory::PluginFactory()
{
#ifdef USE_PYTHON
  pymod = nullptr;
#endif
  for (QStaticPlugin p : QPluginLoader::staticPlugins())
  {
    QString pluginname = "";
    if (!p.metaData().value("MetaData").isUndefined())
    {
      QJsonObject metadata{ p.metaData().value("MetaData").toObject() };
      QJsonValue name = metadata.value("name");
      if (!name.isUndefined()) pluginname = name.toString();
    }
    LibraryData core;
    core.name = pluginname;
    core.instance = p.instance();
    libraryObjects.insert(std::make_pair(core.name,core));
    initPlugin(core.instance);
    std::cout << core.name.toStdString() << std::endl;
  }
  loadPlugins();
}

PluginFactory::~PluginFactory()
{
}

#ifdef USE_PYTHON
void PluginFactory::setPythonBinding(void* m)
{
  pymod = m;
  for (const auto& entry : libraryObjects)
  {
    if (entry.second.instance)
    {
      OpPlugin* p = dynamic_cast<OpPlugin*>(entry.second.instance);
      if (p)
      {
        p->bindPython(m);
      }
      const OpPluginCollection* col = dynamic_cast<const OpPluginCollection*>(entry.second.instance);
      if (col)
      {
        for (OpPlugin* p : col->getPlugins())
        {
          if (pymod) p->bindPython(pymod);
        }
      }
    }
  }
}
#endif

std::vector<OpPlugin*> PluginFactory::getOpPlugins()
{
  std::vector<OpPlugin*> list;
  for (const auto& entry : libraryObjects)
  {
    if (entry.second.instance)
    {
      OpPlugin* p = dynamic_cast<OpPlugin*>(entry.second.instance);
      if (p)
      {
        list.push_back(p);
      }
      const OpPluginCollection* col = dynamic_cast<const OpPluginCollection*>(entry.second.instance);
      if (col)
      {
        auto plugins = col->getPlugins();
        list.insert(list.end(),plugins.begin(),plugins.end());
      }
    }
  }
  return list;
}

QObject* PluginFactory::loadPlugin(QString filename)
{
  std::cout << "try to load: " << filename.toStdString() << std::endl;
  LibraryData lib = initLibrary(filename);
  if (!lib.name.isEmpty())
  {
    if (libraryObjects.find(lib.name) == libraryObjects.end())
    {
      libraryObjects.insert(std::make_pair(lib.name,lib));
      return loadPlugin(libraryObjects.at(lib.name));
    }
    else
    {
      qWarning() << "multiple libraries found for " << lib.name;
    }
  }
  return nullptr;
}

std::vector<LibraryInfo> PluginFactory::getLibraryInfo() const
{
  std::vector<LibraryInfo> list;
  for (const auto& entry : libraryObjects)
  {
    LibraryInfo info;
    info.name = entry.second.name;
    info.filename = entry.second.filename;
    list.push_back(info);
  }
  return list;
}

QStringList PluginFactory::getSearchPath() const
{
  return QString(pathList).split(";");
}




QObject* PluginFactory::loadPlugin(LibraryData& lib)
{
  std::cout << "Load plugin: " << lib.name.toStdString() << std::endl;
  if (lib.instance)
  {
    return lib.instance;
  }
  /* check dependencies */
  for (const QString& dep : lib.dependencies)
  {
    if (libraryObjects.find(dep) == libraryObjects.end())
    {
      qWarning() << lib.name << "failed:" << "dependency not found:" << dep;
      return nullptr;
    }
    /* if not already loaded: load it */
    if (!libraryObjects.at(dep).instance)
    {
      auto p = loadPlugin(libraryObjects.at(dep));
      if (!p)
      {
        qWarning() << lib.name << "failed:" << "dependency not loaded";
        return nullptr;
      }
    }
  }
  /* load plugin */
  bool ret = lib.loader->load();
  if (!ret)
  {
    qWarning() << lib.name << "failed:" << lib.loader->errorString();
    return nullptr;
  }
  lib.filename = lib.loader->fileName();
  lib.instance = lib.loader->instance();
  initPlugin(lib.instance);
  qInfo() << lib.name << "loaded...";
  return lib.instance;
}

void PluginFactory::initPlugin(QObject* o)
{
  const OpPlugin* p = dynamic_cast<const OpPlugin*>(o);
  if (p)
  {
#ifdef USE_PYTHON
    if (pymod) p->bindPython(pymod);
#endif
    connect(p,&OpPlugin::logOperation,this,&PluginFactory::logOperation);
    connect(p,&OpPlugin::logProfilerResult,this,&PluginFactory::logProfilerResult);
  }
  const OpPluginCollection* col = dynamic_cast<const OpPluginCollection*>(o);
  if (col)
  {
    for (OpPlugin* p : col->getPlugins())
    {
#ifdef USE_PYTHON
      if (pymod) p->bindPython(pymod);
#endif
      connect(p,&OpPlugin::logOperation,this,&PluginFactory::logOperation);
      connect(p,&OpPlugin::logProfilerResult,this,&PluginFactory::logProfilerResult);
    }
  }
}

void PluginFactory::loadPlugins()
{
  std::cout << pathList << std::endl;
  QStringList pathlist = QString(pathList).split(";");
  for (const QString& path : pathlist)
  {
    findPlugins(path);
  }
  for (auto& entry : libraryObjects)
  {
    LibraryData& lib = entry.second;
    if (!lib.instance)
    {
      loadPlugin(lib);
    }
  }
}

std::vector<QString> PluginFactory::findPlugins(QString path)
{
//  std::cout << path.toStdString() << std::endl;
  std::vector<QString> list;
  QDir d(path);
  if (!d.exists())
  {
    std::cout << path.toStdString() << " does not exist" << std::endl;
    return list;
  }
  auto entries = d.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs);
//  std::cout << "entries: " << entries.size() << std::endl;
  for (const auto& entry : entries)
  {
//    std::cout << entry.absoluteFilePath().toStdString() << std::endl;
    if (entry.isDir())
    {
      auto list1 = findPlugins(d.absoluteFilePath(entry.fileName()));
      list.insert(list.end(),list1.begin(),list1.end());
    }
    else if (entry.isFile() && QLibrary::isLibrary(entry.absoluteFilePath()) && entry.baseName().contains("fits"))
    {
//      std::cout << entry.absoluteFilePath().toStdString() << std::endl;
      LibraryData lib = initLibrary(entry.absoluteFilePath());
//      std::cout << lib.name.toStdString() << std::endl;
      if (!lib.name.isEmpty())
      {
        if (libraryObjects.find(lib.name) == libraryObjects.end())
        {
          libraryObjects.insert(std::make_pair(lib.name,lib));
          list.push_back(entry.absoluteFilePath());
        }
        else
        {
          qWarning() << "multiple libraries found for " << lib.name;
        }
      }
    }
  }
  return list;
}

PluginFactory::LibraryData PluginFactory::initLibrary(QString filename)
{
  LibraryData lib;
  lib.loader = std::make_shared<QPluginLoader>(filename);
  lib.loader->setLoadHints(QLibrary::ExportExternalSymbolsHint|QLibrary::ResolveAllSymbolsHint);
  if (!lib.loader->metaData().value("MetaData").isUndefined())
  {
    QJsonObject metadata{lib.loader->metaData().value("MetaData").toObject()};
    QJsonValue name = metadata.value("name");
    if (!name.isUndefined()) lib.name = name.toString();
    QJsonValue dep = metadata.value("depends");
    lib.dependencies = dep.toString().split(";",Qt::SkipEmptyParts);
  }
  return lib;
}

