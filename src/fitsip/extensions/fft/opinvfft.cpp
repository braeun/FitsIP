/********************************************************************************
 *                                                                              *
 * FitsIP - calculate the inverse FFT                                           *
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

#include "opinvfft.h"
#include <fitsip/core/fitsimage.h>
#include <fftw3.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

/**
 * @note: FFTW requires arrays in row-major (or C) format with the last
 * dimension changing the fastest. For the images, this is the width (x)
 * dimension. Thus for calling FFTW routines, the dimensions must be passed
 * in the order height, width.
 */

OpInvFFT::OpInvFFT()
{
  profiler = SimpleProfiler("OpInvFFT");
}

OpInvFFT::~OpInvFFT()
{
}

std::vector<std::shared_ptr<FitsObject>> OpInvFFT::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString OpInvFFT::getMenuEntry() const
{
  return "Math/Inverse FFT";
}

#ifdef USE_PYTHON
void OpInvFFT::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("invfft",[this](std::shared_ptr<FitsObject> obj){
    auto img = invfft(obj->getImage());
    return std::make_shared<FitsObject>(img);
  },
  "Calculate inverse FFT of the image",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpInvFFT::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (image->getImage().getDepth() != 2)
  {
    setError("Input is not a complex image");
    img = std::shared_ptr<FitsImage>();
    return ERROR;
  }
  else
  {
    profiler.start();
    FitsImage tmp;
    if (data.aoi.isNull())
      tmp = image->getImage();
    else
      tmp = image->getImage().subImage(data.aoi);
    img = invfft(tmp);
    profiler.stop();
  }
  logProfiler(*img);
  return OK;
}

std::shared_ptr<FitsImage> OpInvFFT::invfft(const FitsImage& image) const
{
  int preFFTWidth = (image.getWidth() - 1) * 2;
  int preFFTHeight = image.getHeight();
  fftw_complex* in = new fftw_complex[image.getHeight()*image.getWidth()];
  double *out = new double[preFFTHeight*preFFTWidth];
  fftw_plan f = fftw_plan_dft_c2r_2d(preFFTHeight,preFFTWidth,in,out,FFTW_ESTIMATE);
  ConstPixelIterator it = image.getConstPixelIterator();
  fftw_complex* cptr = in;
  for (int i=0;i<image.getHeight()*image.getWidth();i++)
  {
    (*cptr)[0] = it[0];
    (*cptr)[1] = it[1];
    ++it;
    ++cptr;
  }
  fftw_execute(f);
  auto fftimg = std::make_shared<FitsImage>(image.getName()+"_INVFFT",preFFTWidth,preFFTHeight,1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = out;
  for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
  {
    it2[0] = *ptr;
    ++ptr;
    ++it2;
  }
  fftw_destroy_plan(f);
  delete [] out;
  delete [] in;
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  return fftimg;
}
