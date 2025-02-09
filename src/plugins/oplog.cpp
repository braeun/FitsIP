/********************************************************************************
 *                                                                              *
 * FitsIP - scale image by logarithm                                            *
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

#include "oplog.h"
#include <fitsbase/fitsimage.h>
#include <cmath>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpLog::OpLog()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpLog");
}

OpLog::~OpLog()
{
}

QString OpLog::getMenuEntry() const
{
  return "Math/Log";
}

QIcon OpLog::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/oplog.png");
}

#ifdef USE_PYTHON
void OpLog::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("log",[this](std::shared_ptr<FitsObject> obj){
    calcLog(obj->getImage());
    obj->getImage()->log("log10 of image");
    return OK;
  },
  "Take logarithm of image",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpLog::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  profiler.start();
  calcLog(image->getImage());
  profiler.stop();
  log(image,"Log10 of image");
  logProfiler(image);
  return OK;
}

void OpLog::calcLog(std::shared_ptr<FitsImage> img) const
{
  PixelIterator p = img->getPixelIterator();
  if (img->getDepth() == 1)
  {
    while (p.hasNext())
    {
      if (p[0] <= 0)
        p[0] = 0;
      else
        p[0] = log10(p[0]);
      ++p;
    }
  }
  else
  {
    while (p.hasNext())
    {
      ValueType val = p.getAbs();
      ValueType nval = log10(val);
      for (int d=0;d<img->getDepth();d++) p[d] = p[d] * nval / val;
      ++p;
    }
  }
}


