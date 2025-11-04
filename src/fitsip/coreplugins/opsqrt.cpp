/********************************************************************************
 *                                                                              *
 * FitsIP - scale image intensity by square root                                *
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

#include "opsqrt.h"
#include <fitsip/core/fitsimage.h>
#include <cmath>
#include <iostream>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpSqrt::OpSqrt()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpSqrt");
}

OpSqrt::~OpSqrt()
{
}

QString OpSqrt::getMenuEntry() const
{
  return "Math/Sqrt";
}

QIcon OpSqrt::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/opsqrt.png");
}

#ifdef USE_PYTHON
void OpSqrt::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("sqrt",[this](std::shared_ptr<FitsObject> obj){
    calcSqrt(obj->getImage());
    obj->getImage()->log("Square Root of image");
    return OK;
  },
  "Take square root of image",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpSqrt::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  profiler.start();
  calcSqrt(image->getImage());
  profiler.stop();
  log(image,"Square Root of image");
  logProfiler(image);
  return OK;
}

void OpSqrt::calcSqrt(FitsImage* img) const
{
  PixelIterator p = img->getPixelIterator();
  if (img->getDepth() == 1)
  {
    while (p.hasNext())
    {
      if (p[0] <= 0)
        p[0] = 0;
      else
        p[0] = sqrt(p[0]);
      ++p;
    }
  }
  else
  {
    while (p.hasNext())
    {
      ValueType val = p.getAbs();
      ValueType nval = sqrt(val);
      for (int d=0;d<img->getDepth();d++) p[d] = p[d] * nval / val;
      ++p;
    }
  }
}

