/********************************************************************************
 *                                                                              *
 * FitsIP - apply Sobel filter                                                  *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#include "opsobel.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/kernelrepository.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpSobel::OpSobel()
{
}

OpSobel::~OpSobel()
{
}

QString OpSobel::getMenuEntry() const
{
  return "Filter/Edge";
}

#ifdef USE_PYTHON
void OpSobel::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("sobel",[this](std::shared_ptr<FitsObject> obj){
    auto img = applyFilter(obj->getImage());
    obj->setImage(img);
    obj->getImage()->log("Applied edge (Sobel) filter");
    return OK;
  },
  "Apply edge (Sobel) filter",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpSobel::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  profiler.start();
  auto img = applyFilter(image->getImage());
  image->setImage(img);
  profiler.stop();
  log(image,"Applied edge (Sobel) filter");
  logProfiler(image);
  return ResultType::OK;
}


std::shared_ptr<FitsImage> OpSobel::applyFilter(std::shared_ptr<FitsImage>  img) const
{
  const Kernel& kx = KernelRepository::instance().getKernel(KernelRepository::SOBEL_X);
  ValueType* gx = convolve(img,kx);
  const Kernel& ky = KernelRepository::instance().getKernel(KernelRepository::SOBEL_Y);
  ValueType* gy = convolve(img,ky);
  auto img1 = std::make_shared<FitsImage>(img->getName(),img->getWidth(),img->getHeight(),1);
  img1->setMetadata(img->getMetadata());
  PixelIterator it = img1->getPixelIterator();
  ValueType* ptrgx = gx;
  ValueType* ptrgy = gy;
  uint32_t n = img->getWidth() * img->getHeight();
  while (n-- > 0)
  {
    it[0] = sqrt(*ptrgx * *ptrgx + *ptrgy * *ptrgy);
    ++it;
    ++ptrgx;
    ++ptrgy;
  }
  delete [] gx;
  delete [] gy;
  return img1;
}

ValueType* OpSobel::convolve(std::shared_ptr<FitsImage> img, const Kernel &kernel) const
{
  ValueType* g = new ValueType[img->getWidth()*img->getHeight()];
  memset(g,0,img->getWidth()*img->getHeight()*sizeof(ValueType));
  ValueType* ptr_g = g;
  uint32_t wk2 = kernel.getWidth() / 2;
  uint32_t hk2 = kernel.getHeight() / 2;
  ptr_g += hk2 * img->getWidth();
  for (uint32_t y0=hk2;y0<img->getHeight()-hk2;y0++)
  {
    ptr_g += wk2;
    for (uint32_t x0=wk2;x0<img->getWidth()-wk2;x0++)
    {
      uint32_t y = y0 - hk2;
      for (uint32_t rk=0;rk<kernel.getHeight();rk++)
      {
        ConstPixelIterator it2 = img->getConstPixelIterator(x0-wk2,y);
        for (uint32_t ck=0;ck<kernel.getWidth();ck++)
        {
          *ptr_g += kernel[rk][ck] * it2.getAbs();
          ++it2;
        }
        y++;
      }
      ++ptr_g;
    }
    ptr_g += wk2;
  }
  return g;
}
