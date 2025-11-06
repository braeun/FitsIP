/********************************************************************************
 *                                                                              *
 * FitsIP - unsharp masking                                                     *
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

#include "opunsharpmask.h"
#include "opunsharpmaskdialog.h"
#include "opgaussblur.h"
#include <fitsip/core/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpUnsharpMask::OpUnsharpMask():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpUnsharpMask");
}

OpUnsharpMask::~OpUnsharpMask()
{
}

QString OpUnsharpMask::getMenuEntry() const
{
  return "Filter/Unsharp Mask...";
}

#ifdef USE_PYTHON
void OpUnsharpMask::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("unsharp_mask",[this](std::shared_ptr<FitsObject> obj, ValueType sigma, ValueType strength){
    unsharpmask(&obj->getImage(),sigma,strength);
    obj->getImage().log(QString::asprintf("Unsharp mask: sigma=%f  strength=%f",sigma,strength));
    return OK;
  },
  "Apply unsharp mask",py::arg("obj"),py::arg("sigma"),py::arg("strength"));
}
#endif

OpPlugin::ResultType OpUnsharpMask::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new OpUnsharpMaskDialog();
  dlg->setSourceImage(image->getImage(),data.aoi,data.previewOptions);
  if (dlg->exec())
  {
    ValueType sigma = dlg->getSigma();
    ValueType strength = dlg->getStrength();
    profiler.start();
    unsharpmask(&image->getImage(),sigma,strength);
    profiler.stop();
    log(image,QString::asprintf("Unsharp mask: sigma=%f  strength=%f",sigma,strength));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpUnsharpMask::unsharpmask(FitsImage* image, ValueType sigma, ValueType strength) const
{
  OpGaussBlur blur;
  auto blurred = std::make_shared<FitsImage>(*image);
  blur.blur(blurred.get(),sigma,sigma);
  for (int d=0;d<image->getDepth();d++)
  {
    ValueType *p1 = image->getLayer(d).getData();
    const ValueType *p2 = blurred->getLayer(d).getData();
    int n = image->getWidth() * image->getHeight();
    while (n-- > 0)
    {
      *p1 = (1 + strength) * *p1 - strength * *p2;
      ++p1;
      ++p2;
    }
  }
}


