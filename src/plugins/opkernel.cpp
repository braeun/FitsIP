/********************************************************************************
 *                                                                              *
 * FitsIP - kernel filter                                                       *
 *                                                                              *
 * modified: 2025-03-18                                                         *
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

#include "opkernel.h"
#include "opkerneldialog.h"
#include <fitsbase/fitsimage.h>
#include <fitsbase/kernelrepository.h>
#include <iostream>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpKernel::OpKernel():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpKernel");
}

OpKernel::~OpKernel()
{
  if (dlg) dlg->deleteLater();
}

QString OpKernel::getMenuEntry() const
{
  return "Filter/Kernel Convolution...";
}

#ifdef USE_PYTHON
void OpKernel::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("convolve",[this](std::shared_ptr<FitsObject> obj, const Kernel& kernel){
    convolve(obj->getImage(),kernel);
    obj->getImage()->log("Convolution with kernel "+kernel.getName());
    return OK;
  },
  "Convolution with kernel",py::arg("obj"),py::arg("kernel"));
}
#endif

OpPlugin::ResultType OpKernel::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new OpKernelDialog();
  dlg->setSourceImage(image->getImage(),data.aoi,data.previewOptions);
  dlg->setKernelNames(KernelRepository::instance().getKernelNames());
  if (!dlg->exec()) return CANCELLED;
  Kernel kernel = KernelRepository::instance().getKernel(dlg->getKernelName());
  if (kernel.isEmpty()) return ERROR;
  profiler.start();
  convolve(image->getImage(),kernel);
  profiler.stop();
  log(image,"Convolution with kernel "+kernel.getName());
  logProfiler(image);
  return OK;
}

void OpKernel::convolve(std::shared_ptr<FitsImage> image, const Kernel& kernel) const
{
  FitsImage tmp(*image);
  PixelIterator it1 = image->getPixelIterator();
  uint32_t wk2 = kernel.getWidth() / 2;
  uint32_t hk2 = kernel.getHeight() / 2;
  it1 += hk2 * image->getWidth();
  for (uint32_t y0=hk2;y0<image->getHeight()-hk2;y0++)
  {
    it1 += wk2; /* start half kernel width from beginning of line */
    for (uint32_t x0=wk2;x0<image->getWidth()-wk2;x0++)
    {
      it1.clear();
      uint32_t y = y0 - hk2;
      for (uint32_t rk=0;rk<kernel.getHeight();rk++)
      {
        ConstPixelIterator it2 = tmp.getConstPixelIterator(x0-wk2,y);
        for (uint32_t ck=0;ck<kernel.getWidth();ck++)
        {
          for (int d=0;d<image->getDepth();d++)
          {
            it1[d] += kernel[rk][ck] * it2[d];
          }
          ++it2;
        }
        y++;
      }
      ++it1;
    }
    it1 += wk2; /* skip half kernel width to end of line */
  }
}

