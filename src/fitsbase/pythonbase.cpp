/********************************************************************************
 *                                                                              *
 * FitsIP - python bindings                                                     *
 *                                                                              *
 * modified: 2025-02-01                                                         *
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

#include "pythonbase.h"
#include "fitsobject.h"
#include "imagestatistics.h"
#include "kernel.h"
#include "kernelrepository.h"
#include "opplugin.h"
#include "scriptutilities.h"
#include <pybind11/stl.h>

namespace pythonbase {

void bindFitsObject(py::module_& m)
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

void bindKernel(py::module_& m)
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


void bindStatistics(py::module_& m)
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


void bind(py::module_& m)
{
  bindFitsObject(m);
  bindKernel(m);
  bindStatistics(m);

//  py::class_<OpPlugin> opplugin(m,"OpPlugin","Operation Plugin");
  py::enum_<OpPlugin::ResultType>(m,"ResultType", py::arithmetic())
      .value("OK",OpPlugin::ResultType::OK)
      .value("ERROR",OpPlugin::ResultType::ERROR)
      .value("CANCELLED",OpPlugin::ResultType::CANCELLED)
      .export_values()
      ;

  m.def("clone",[](std::shared_ptr<FitsObject> obj, const std::string& filename){return obj->clone(filename);},
    "Clone an image",py::arg("obj"),py::arg("filename"));
}

}

