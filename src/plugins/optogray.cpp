/********************************************************************************
 *                                                                              *
 * FitsIP - convert image to gray scale image                                   *
 *                                                                              *
 * modified: 2025-02-06                                                         *
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

#include "optogray.h"
#include <fitsbase/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpToGray::OpToGray()
{
  profiler = SimpleProfiler("OpToGray");
}

OpToGray::~OpToGray()
{
}

QString OpToGray::getMenuEntry() const
{
  return "Color/To Gray";
}

#ifdef USE_PYTHON
void OpToGray::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("to_gray",[this](std::shared_ptr<FitsObject> obj){
    if (obj->getImage()->getDepth() != 3)
    {
      return ERROR;
    }
    obj->setImage(toGray(obj)->getImage());
    return OK;
  },
  "Convert image to gray",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpToGray::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  if (image->getImage()->getDepth() != 3)
  {
    setError("Not a color image");
    return ERROR;
  }
  profiler.start();
  image->setImage(toGray(image)->getImage());
  profiler.stop();
  log(image,"Converted to gray image");
  logProfiler(image);
  return OK;
}

std::shared_ptr<FitsObject>  OpToGray::toGray(std::shared_ptr<FitsObject> image) const
{
  return std::make_shared<FitsObject>(image->getImage()->toGray());
}
