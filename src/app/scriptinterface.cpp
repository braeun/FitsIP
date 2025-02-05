/********************************************************************************
 *                                                                              *
 * FitsIP - interface function for scripts                                      *
 *                                                                              *
 * modified: 2025-02-04                                                         *
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

#include "scriptinterface.h"
#include <fitsbase/fitsobject.h>
#include <fitsbase/imagecollection.h>
#include <fitsbase/io/iofactory.h>
#include <QFileInfo>
#include <QDebug>

#ifdef USE_PYTHON
#include <pybind11/embed.h>
#endif

std::unique_ptr<ScriptInterface> ScriptInterface::interface;

ScriptInterface::ScriptInterface():QObject()
{
}

void ScriptInterface::setWorkingDir(const QString& dir)
{
  workingdir = dir;
}

void ScriptInterface::changeWorkingDir(const std::string& dir)
{
  workingdir = QString::fromStdString(dir);
  emit workingDirChanged(workingdir);
}

std::shared_ptr<FitsObject> ScriptInterface::get(const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(workingdir+"/"+QString::fromStdString(filename));
  }
  return ImageCollection::getGlobal().getFile(fileinfo.absoluteFilePath());
}

std::shared_ptr<FitsObject> ScriptInterface::load(const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(workingdir+"/"+QString::fromStdString(filename));
  }
  IOHandler* handler = IOFactory::getInstance()->getHandler(fileinfo.absoluteFilePath());
  if (handler)
  {
    try
    {
      std::shared_ptr<FitsImage> image = handler->read(fileinfo.absoluteFilePath());
      std::shared_ptr<FitsObject> file = std::make_shared<FitsObject>(image,fileinfo.absoluteFilePath());
      return file;
    }
    catch (std::exception& ex)
    {
      qCritical() << ex.what();
    }
  }
  else
  {
    qWarning() << "No IOHandler for " << fileinfo.absoluteFilePath();
  }
  return std::shared_ptr<FitsObject>();
}

bool ScriptInterface::save(std::shared_ptr<FitsObject> obj, const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(workingdir+"/"+QString::fromStdString(filename));
  }
  try
  {
    obj->save(fileinfo.absoluteFilePath());
//    logbook.add(LogbookEntry::Op,activeFile->getImage()->getName(),"Saved to file "+fn);
  }
  catch (std::exception& ex)
  {
    qCritical() << ex.what();
    return false;
  }
  return true;
}


void ScriptInterface::display(std::shared_ptr<FitsObject> obj)
{
  std::shared_ptr<FitsObject> loaded = ImageCollection::getGlobal().getFile(obj->getId());
  if (!loaded)
  {
    ImageCollection::getGlobal().addFile(obj);
  }
  emit display(obj->getId());
}


#ifdef USE_PYTHON

void ScriptInterface::bind(py::module_& m)
{
  m.def("display",[](std::shared_ptr<FitsObject> obj){ScriptInterface::getInterface()->display(obj);},
    "Display an image",py::arg("obj"));
  m.def("get_cwd",[](){return ScriptInterface::getInterface()->workingdir.toStdString();},
    "Get the current working directory");
  m.def("get",[](const std::string& filename){return ScriptInterface::getInterface()->get(filename);},
      "Get an already opened image from the display list",py::arg("filename"));
  m.def("load",[](const std::string& filename){return ScriptInterface::getInterface()->load(filename);},
      "Load an image from disk",py::arg("filename"));
  m.def("save",[](std::shared_ptr<FitsObject> obj){return ScriptInterface::getInterface()->save(obj,obj->getFilename().toStdString());},
    "Save an image under the name it was loaded with",py::arg("obj"));
  m.def("save",[](std::shared_ptr<FitsObject> obj, const std::string& filename){return ScriptInterface::getInterface()->save(obj,filename);},
    "Save an image under a new name",py::arg("obj"),py::arg("filename"));
  m.def("set_cwd",[](const std::string& dir){ScriptInterface::getInterface()->changeWorkingDir(dir);},
    "Set the current working directory");
  m.attr("a") = 1;
}

#endif

ScriptInterface* ScriptInterface::getInterface()
{
  if (!interface)
  {
    interface.reset(new ScriptInterface());
  }
  return interface.get();
}
