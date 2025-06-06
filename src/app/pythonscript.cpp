/********************************************************************************
 *                                                                              *
 * FitsIP - python scripting                                                    *
 *                                                                              *
 * modified: 2025-05-30                                                         *
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

#include "pythonscript.h"
#include "scriptinterface.h"
#include <fitsip/core/filelist.h>
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/kernel.h>
#include <fitsip/core/kernelrepository.h>
#include <fitsip/core/opplugin.h>
#include <fitsip/core/pluginfactory.h>
#include <fitsip/core/scriptutilities.h>
#include <iostream>
#include <functional>
#include <QApplication>
#include <QFileInfo>
#include <pybind11/stl.h>

PYBIND11_EMBEDDED_MODULE(fits, m) {
  m.attr("scriptpath") = "";
}



void PythonScript::runCmd(const QString& cmd)
{
//  std::cout << "python: " << cmd.toStdString() << std::endl;
  auto fits_module = py::module_::import("fits");
  fits_module.attr("scriptpath") = "";
  py::exec(cmd.toStdString());
}

void PythonScript::runFile(const QString& filename)
{
  QFileInfo info(filename);
  auto fits_module = py::module_::import("fits");
  fits_module.attr("scriptpath") = info.absolutePath().toStdString();
  py::eval_file(filename.toStdString());
}

PythonScript::PythonScript(ScriptInterface *intf, PluginFactory* plugins)
{
  init(intf,plugins);
}

PythonScript::~PythonScript()
{
}

void PythonScript::init(ScriptInterface* intf, PluginFactory* plugins)
{
  if (terp) terp.reset();
  terp.reset(new py::scoped_interpreter);
  redirect(pybind11::cpp_function([this](const char* s){streamStdout(s);}),"stdout");
  redirect(pybind11::cpp_function([this](const char* s){streamStderr(s);}),"stderr");
  auto m = py::module_::import("fits");
  bind(m,intf);
  plugins->setPythonBinding(&m);
  py::exec("import fits");
}

void PythonScript::streamStdout(const char* s)
{
  emit stdoutAvailable(s);
  QApplication::processEvents();
}

void PythonScript::streamStderr(const char* s)
{
  emit stderrAvailable(s);
  QApplication::processEvents();
}


/*
 * based on
 * https://github.com/TinyTinni/PyLogHook
 * Copyright(c) 2017 Matthias Möller
 * MIT License
 */

void PythonScript::redirect(py::object callback, const char* pipe)
{
  PyObject *out = PySys_GetObject(pipe);

  // in case python could not create stdout/stderr
  // this happens in some gui application
  // just register a new nameless type for this
  if (out == nullptr || out == Py_None)
  {
      std::string register_read_write = std::string("import sys\n\
sys.") + pipe + std::string(" = type(\"\",(object,),{\"write\":lambda self, txt: None, \"flush\":lambda self: None})()\n");
      PyRun_SimpleString(register_read_write.c_str());
      out = PySys_GetObject(pipe);
  }
  // overwrite write function
  PyObject_SetAttrString(out, "write",callback.ptr());
}

void PythonScript::bind(py::module_& m, ScriptInterface* intf)
{
  m.def("display",[intf](std::shared_ptr<FitsObject> obj){intf->display(obj);},
    "Display an image",py::arg("obj"));
  m.def("get_cwd",[intf](){return intf->getWorkingDir();},
    "Get the current working directory");
  m.def("get",[intf](const std::string& filename){return intf->get(filename);},
      "Get an already opened image from the display list",py::arg("filename"));
  m.def("get_open",[intf](){return intf->getOpen();},
      "Get an already opened image from the display list");
  m.def("get_selected",[intf](){return intf->getSelectedFileList();},
      "Get the list ocontainer of selected files");
  m.def("load",[intf](const std::string& filename){return intf->load(filename);},
      "Load an image from disk",py::arg("filename"));
  m.def("save",[intf](std::shared_ptr<FitsObject> obj){return intf->save(obj,obj->getFilename().toStdString());},
    "Save an image under the name it was loaded with",py::arg("obj"));
  m.def("save",[intf](std::shared_ptr<FitsObject> obj, const std::string& filename){return intf->save(obj,filename);},
    "Save an image under a new name",py::arg("obj"),py::arg("filename"));
  m.def("set_cwd",[intf](const std::string& dir){intf->setWorkingDir(dir);},
    "Set the current working directory");

  bindFitsObject(m);
  bindLists(m,intf);
  bindKernel(m);
  bindStatistics(m);

  //  py::class_<OpPlugin> opplugin(m,"OpPlugin","Operation Plugin");
  py::enum_<OpPlugin::ResultType>(m,"ResultType", py::arithmetic())
      .value("OK",OpPlugin::ResultType::OK)
      .value("ERROR",OpPlugin::ResultType::ERROR)
      .value("CANCELLED",OpPlugin::ResultType::CANCELLED)
      .export_values()
      ;

  m.def("copy",[](std::shared_ptr<FitsObject> obj, const std::string& filename){return obj->copy(filename);},
      "Copy an image",py::arg("obj"),py::arg("filename"));
}

void PythonScript::bindLists(py::module_& m, ScriptInterface* intf)
{
  py::class_<FileList, std::shared_ptr<FileList>>(m,"FileList","List of files")
      .def(py::init<>())
      .def("clear",&FileList::clear)
      .def("get_files",[](FileList& list){
          std::vector<std::string> l;
          for (const QFileInfo& i : list.getFiles()) l.push_back(i.absoluteFilePath().toStdString());
          return l;
        },"Return list of files")
      .def("add",[intf](FileList& list, const std::string& file){
          QFileInfo info(QString::fromStdString(file));
          if (!info.isAbsolute()) info = QFileInfo(QString::fromStdString(intf->getWorkingDir()+"/"+file));
          list.addFile(info);},
          "Add a file to the list",py::arg("file"))
      ;
}

void PythonScript::bindFitsObject(py::module_& m)
{
  py::class_<FitsObject, std::shared_ptr<FitsObject>>(m,"FitsObject","Fits Object")
      .def(py::init<std::shared_ptr<FitsImage>, const std::string&>())
      .def_property_readonly("id",&FitsObject::getId)
      .def_property_readonly("name",[](const FitsObject& obj){return obj.getName().toStdString();})
      .def_property_readonly("filename",[](const FitsObject& obj){return obj.getFilename().toStdString();})
      .def_property_readonly("width",[](const FitsObject& obj){ return obj.getImage()->getWidth(); },
          "width of image")
      .def_property_readonly("height",[](const FitsObject& obj){ return obj.getImage()->getHeight(); },
          "height of image")
      .def_property_readonly("depth",[](const FitsObject& obj){ return obj.getImage()->getDepth(); },
          "depth of image")
      .def("push_undo",&FitsObject::pushUndo)
      .def("pop_undo",&FitsObject::popUndo)
      .def("can_undo",&FitsObject::isUndoAvailable)
      .def("log",[](const std::shared_ptr<FitsObject>& obj, const std::string& msg){
            obj->getImage()->log(QString::fromStdString(msg));
          },
          "Add a message to the history",py::arg("msg"))
      ;
}

void PythonScript::bindKernel(py::module_& m)
{
  py::class_<Kernel, std::shared_ptr<Kernel>>(m,"Kernel","Convolution kernel")
      .def(py::init<>())
      .def(py::init([](const std::string& name, const std::string& type, uint32_t w, uint32_t h) {
        return new Kernel(QString::fromStdString(name),QString::fromStdString(type),w,h);
      }))
      .def(py::init([](const std::string& name, const std::string& type, uint32_t w, uint32_t h, const std::vector<std::vector<ValueType>>& k) {
        return new Kernel(QString::fromStdString(name),QString::fromStdString(type),w,h,k);
      }))
      .def("get",[](const Kernel& k, uint32_t row, uint32_t col){ return k[row][col];},
          "Return an element of the kernel",py::arg("row"),py::arg("col"))
      .def_property_readonly("kernel",&Kernel::getKernel)
      .def_property_readonly("name",[](const Kernel& k) {return k.getName().toStdString();})
      .def_property_readonly("type",[](const Kernel& k) {return k.getType().toStdString();})
      .def_property_readonly("width",&Kernel::getWidth)
      .def_property_readonly("height",&Kernel::getHeight)
      .def("is_empty",&Kernel::isEmpty)
      .def("normalize",&Kernel::normalize)
      ;

  py::class_<KernelRepository>(m,"KernelRepository","Kernel repository")
      .def_readonly_static("BOXCAR3",&KernelRepository::BOXCAR3)
      .def_readonly_static("BOXCAR5",&KernelRepository::BOXCAR5)
      .def_readonly_static("BLUR50",&KernelRepository::BLUR50)
      .def_readonly_static("GENTLEBLUR",&KernelRepository::GENTLEBLUR)
      .def_readonly_static("MINIMALBLUR",&KernelRepository::MINIMALBLUR)
      .def_readonly_static("GAUSSIAN3",&KernelRepository::GAUSSIAN3)
      .def_readonly_static("GAUSSIAN5",&KernelRepository::GAUSSIAN5)
      .def_readonly_static("DONUT",&KernelRepository::DONUT)
      .def_readonly_static("TRIANGLE5",&KernelRepository::TRIANGLE5)
      .def_readonly_static("SHARPEN",&KernelRepository::SHARPEN)
      .def_readonly_static("SHARPEN_GAUSSIAN",&KernelRepository::SHARPEN_GAUSSIAN)
      .def_readonly_static("CRISPEN1",&KernelRepository::CRISPEN1)
      .def_readonly_static("CRISPEN2",&KernelRepository::CRISPEN2)
      .def_readonly_static("SOBEL_X",&KernelRepository::SOBEL_X)
      .def_readonly_static("SOBEL_Y",&KernelRepository::SOBEL_Y)
      .def_readonly_static("LAPLACIAN",&KernelRepository::LAPLACIAN)
      .def_static("get_kernel",[](const std::string& name){return KernelRepository::instance().getKernel(QString::fromStdString(name));})
      .def_static("get_names",[](){return ScriptUtilities::convertStringList(KernelRepository::instance().getKernelNames());})
      ;
}

void PythonScript::bindStatistics(py::module_& m)
{
  py::class_<LayerStatistics>(m,"LayerStatistics","Statistics of an image layer")
      .def(py::init<>())
      .def_readwrite("min",&LayerStatistics::minValue)
      .def_readwrite("max",&LayerStatistics::maxValue)
      .def_readwrite("mean",&LayerStatistics::meanValue)
      .def_readwrite("stddev",&LayerStatistics::stddev)
      ;
  py::class_<ImageStatistics>(m,"ImageStatistics","Statistics of an image")
      .def(py::init<>())
      .def(py::init([](const FitsImage& img){
        return new ImageStatistics(img);
      }))
      .def(py::init([](const FitsImage& img, int x, int y, int w, int h){
        return new ImageStatistics(img,QRect(x,y,w,h));
      }))
      .def(py::init([](const FitsObject& img){
        return new ImageStatistics(*img.getImage());
      }))
      .def(py::init([](const FitsObject& img, int x, int y, int w, int h){
        return new ImageStatistics(*img.getImage(),QRect(x,y,w,h));
      }))
      .def_property_readonly("summary",&ImageStatistics::getGlobalStatistics)
      .def_property_readonly("layers",&ImageStatistics::getLayerStatistics)
      ;
}




