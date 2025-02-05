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
#include "opplugin.h"

namespace pythonbase {

void bind(py::module_& m)
{
  py::class_<FitsObject, std::shared_ptr<FitsObject>>(m,"FitsObject","Fits Object")
      .def(py::init<std::shared_ptr<FitsImage>, const std::string&>())
      .def("get_id",&FitsObject::getId)
      .def("get_name",&FitsObject::getNameStd)
      .def("get_filename",&FitsObject::getFilenameStd)
      .def("push_undo",&FitsObject::pushUndo)
      .def("pop_undo",&FitsObject::popUndo)
      .def("can_undo",&FitsObject::isUndoAvailable)
      ;
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

