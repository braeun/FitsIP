/********************************************************************************
 *                                                                              *
 * FitsIP - split channels of a multilayer image                                *
 *                                                                              *
 * modified: 2025-08-27                                                         *
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

#include "opsplitchannels.h"
#include <fitsip/core/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
#endif

OpSplitChannels::OpSplitChannels()
{
  profiler = SimpleProfiler("OpSplitChannels");
}

OpSplitChannels::~OpSplitChannels()
{
}

std::vector<std::shared_ptr<FitsObject>> OpSplitChannels::getCreatedImages() const
{
  std::vector<std::shared_ptr<FitsObject>> list;
  for (auto i : images)
  {
    list.push_back(std::make_shared<FitsObject>(i));
  }
  return list;
}


QString OpSplitChannels::getMenuEntry() const
{
  return "Image/Color/Split";
}

#ifdef USE_PYTHON
void OpSplitChannels::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("split",[this](std::shared_ptr<FitsObject> obj){
    if (obj->getImage().getDepth() != 3)
    {
      return std::vector<std::shared_ptr<FitsObject>>();
    }
    auto images = split(obj->getImage());
    std::vector<std::shared_ptr<FitsObject>> list;
    for (auto i : images)
    {
      list.push_back(std::make_shared<FitsObject>(i));
    }
    return list;
//    return std::make_tuple(list[0],list[1],list[2]);
  },
  "Shift image",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpSplitChannels::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (image->getImage().getDepth() != 3)
  {
    setError("Not a color image");
    return ERROR;
  }
  profiler.start();
  images = split(image->getImage());
  profiler.stop();
  logProfiler(image);
  return OK;
}

std::vector<FitsImage> OpSplitChannels::split(const FitsImage& image) const
{
  std::vector<FitsImage> list{
    FitsImage(image.getName()+"_R",image.getWidth(),image.getHeight(),1),
    FitsImage(image.getName()+"_G",image.getWidth(),image.getHeight(),1),
    FitsImage(image.getName()+"_B",image.getWidth(),image.getHeight(),1)
  };
  list[0].setMetadata(image.getMetadata());
  list[1].setMetadata(image.getMetadata());
  list[2].setMetadata(image.getMetadata());
  ConstPixelIterator src = image.getConstPixelIterator();
  PixelIterator r = list[0].getPixelIterator();
  PixelIterator g = list[1].getPixelIterator();
  PixelIterator b = list[2].getPixelIterator();
  while (src.hasNext())
  {
    r[0] = src[0];
    g[0] = src[1];
    b[0] = src[2];
    ++src;
    ++r;
    ++g;
    ++b;
  }
  return list;
}

