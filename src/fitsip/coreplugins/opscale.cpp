/********************************************************************************
 *                                                                              *
 * FitsIP - linear scaling of image intensity                                   *
 *                                                                              *
 * modified: 2023-01-31                                                         *
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

#include "opscale.h"
#include <fitsip/core/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpScale::OpScale()
{
  dlg.setTitle("Scale Image");
  dlg.setValue1Label("Bias:");
  dlg.setValue2Label("Scale:");
  dlg.setValue1("0.0");
  dlg.setValue2("1.0");
  profiler = SimpleProfiler("OpScale");
}

OpScale::~OpScale()
{
}

QString OpScale::getMenuEntry() const
{
  return "Math/Scale...";
}

#ifdef USE_PYTHON
void OpScale::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("scale",[this](std::shared_ptr<FitsObject> obj, ValueType scale, ValueType bias){
    scaleImage(obj->getImage(),scale,bias);
    obj->getImage()->log(QString("scaled image: scale=%1 bias=%2").arg(scale).arg(bias));
    return OK;
  },
  "Scale an image",py::arg("obj"),py::arg("scale"),py::arg("bias"));
}
#endif

OpPlugin::ResultType OpScale::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (dlg.exec())
  {
    ValueType bias = dlg.getValue1().toDouble();
    ValueType scale = dlg.getValue2().toDouble();
    profiler.start();
    scaleImage(image->getImage(),scale,bias);
    profiler.stop();
    log(image,QString("scaled image: scale=%1 bias=%2").arg(scale).arg(bias));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpScale::scaleImage(FitsImage* img, ValueType scale, ValueType bias) const
{
  int n = img->getWidth() * img->getHeight();
  for (int i=0;i<img->getDepth();i++)
  {
    ValueType* p = img->getLayer(i)->getData();
    for (int j=0;j<n;j++)
    {
      *p = *p * scale + bias;
      p++;
    }
  }
}

