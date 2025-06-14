/********************************************************************************
 *                                                                              *
 * FitsIP - flip image horizontally                                             *
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

#include "opflipx.h"
#include <fitsip/core/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpFlipX::OpFlipX()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpFlipX");
}

OpFlipX::~OpFlipX()
{
}

QString OpFlipX::getMenuEntry() const
{
  return "Image/Flip X";
}

QIcon OpFlipX::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/object-flip-horizontal.png");
}

#ifdef USE_PYTHON
void OpFlipX::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("flipx",[this](std::shared_ptr<FitsObject> obj){
    flip(obj->getImage());
    obj->getImage()->log("flipped in X");
    return OK;
  },
  "Flip in x",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpFlipX::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  profiler.start();
  flip(image->getImage());
  profiler.stop();
  log(image,"flipped in X");
  logProfiler(image);
  return OK;
}

void OpFlipX::flip(std::shared_ptr<FitsImage> img) const
{
  for (int y=0;y<img->getHeight();y++)
  {
    PixelIterator it1 = img->getPixelIterator(0,y);
    PixelIterator it2 = img->getPixelIterator(img->getWidth()-1,y);
    for (int x=0;x<=img->getWidth()/2;x++)
    {
      for (int d=0;d<img->getDepth();d++)
      {
        ValueType v = it1[d];
        it1[d] = it2[d];
        it2[d] = v;
      }
      ++it1;
      --it2;
    }
  }
}
