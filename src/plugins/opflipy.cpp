/********************************************************************************
 *                                                                              *
 * FitsIP - flip image vertically                                               *
 *                                                                              *
 * modified: 2025-02-05                                                         *
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

#include "opflipy.h"
#include <fitsbase/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpFlipY::OpFlipY()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpFlipY");
}

OpFlipY::~OpFlipY()
{
}

QString OpFlipY::getMenuEntry() const
{
  return "Image/Flip Y";
}

QIcon OpFlipY::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/object-flip-vertical.png");
}

#ifdef USE_PYTHON
void OpFlipY::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("flipy",[this](std::shared_ptr<FitsObject> obj){
    flip(obj->getImage());
    return OK;
  },
  "Flip in y",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpFlipY::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  profiler.start();
  flip(image->getImage());
  profiler.stop();
  log(image,"flipped in Y");
  logProfiler(image);
  return OK;
}

void OpFlipY::flip(std::shared_ptr<FitsImage> img) const
{
  for (uint32_t y=0;y<img->getHeight()/2;y++)
  {
    PixelIterator it1 = img->getPixelIterator(0,y);
    PixelIterator it2 = img->getPixelIterator(0,img->getHeight()-1-y);
    for (uint32_t x=0;x<=img->getWidth();x++)
    {
      for (uint32_t d=0;d<img->getDepth();d++)
      {
        ValueType v = it1[d];
        it1[d] = it2[d];
        it2[d] = v;
      }
      ++it1;
      ++it2;
    }
  }
}
