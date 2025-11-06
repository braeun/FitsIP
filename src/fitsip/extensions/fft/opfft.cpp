/********************************************************************************
 *                                                                              *
 * FitsIP - calculate the FFT of an image                                       *
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

#include "opfft.h"
#include <fitsip/core/fitsimage.h>
#include <fftw3.h>

/**
 * @note: FFTW requires arrays in row-major (or C) format with the last
 * dimension changing the fastest. For the images, this is the width (x)
 * dimension. Thus for calling FFTW routines, the dimensions must be passed
 * in the order height, width.
 */

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpFFT::OpFFT()
{
  profiler = SimpleProfiler("OpFFT");
}

OpFFT::~OpFFT()
{
}

std::vector<std::shared_ptr<FitsObject>> OpFFT::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString OpFFT::getMenuEntry() const
{
  return "Math/FFT";
}

#ifdef USE_PYTHON
void OpFFT::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("fft",[this](std::shared_ptr<FitsObject> obj){
    FitsImage tmp(obj->getImage());
    if (tmp.getWidth() % 2 != 0)
    {
      tmp = tmp.paddedImage(tmp.getWidth()+1,tmp.getHeight());
    }
    auto img = fft(tmp);
    return std::make_shared<FitsObject>(img);
  },
  "Calculate FFT of the image",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpFFT::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  profiler.start();
  FitsImage tmp;
  if (data.aoi.isNull())
    tmp = image->getImage();
  else
    tmp = image->getImage().subImage(data.aoi);
  if (tmp.getWidth() % 2 != 0)
  {
    tmp = tmp.paddedImage(tmp.getWidth()+1,tmp.getHeight());
  }
  img = fft(tmp);
  profiler.stop();
  logProfiler(img);
  return OK;
}

FitsImage OpFFT::fft(const FitsImage& image) const
{
  fftw_complex *s2c = new fftw_complex[image.getHeight()*(image.getWidth()/2+1)];
  double *in = new double[image.getHeight()*image.getWidth()];
  fftw_plan f = fftw_plan_dft_r2c_2d(image.getHeight(),image.getWidth(),in,s2c,FFTW_ESTIMATE);
  ConstPixelIterator it = image.getConstPixelIterator();
  double* ptr = in;
  for (int i=0;i<image.getHeight()*image.getWidth();i++)
  {
    *ptr++ = it.getAbs();
    ++it;
  }
  fftw_execute(f);
#if 0
  fftw_complex *tmp1 = new fftw_complex[image.getHeight()*image.getWidth()];
  for (int y=0;y<image.getHeight();y++)
  {
    fftw_complex* ptr = tmp1 + y * image.getWidth();
    fftw_complex* sptr = s2c + y * (image.getWidth()/2+1);
    if (image.getWidth()%2 == 0)
    {
      memcpy(&ptr[image.getWidth()/2-1],sptr,(image.getWidth()/2+1)*sizeof(fftw_complex));
      sptr++;
      for (int x=1;x<image.getWidth()/2;x++)
      {
        memcpy(&ptr[image.getWidth()/2-1-x],sptr,sizeof(fftw_complex));
        sptr++;
      }
    }
    else
    {
      memcpy(&ptr[image.getWidth()/2],sptr,(image.getWidth()/2+1)*sizeof(fftw_complex));
      sptr++;
      for (int x=1;x<image.getWidth()/2+1;x++)
      {
        memcpy(&ptr[image.getWidth()/2-x],sptr,sizeof(fftw_complex));
        sptr++;
      }
    }
  }
  fftw_complex *tmp = new fftw_complex[image.getHeight()*image.getWidth()];
  if (image.getHeight()%2 == 0)
  {
    int size = image.getHeight() / 2 * image.getWidth();
    memcpy(tmp,tmp1+size,size*sizeof(fftw_complex));
    memcpy(tmp+size,tmp1,size*sizeof(fftw_complex));
  }
  else
  {
    int size = (image.getHeight() / 2 + 1) * image.getWidth();
    memcpy(tmp,tmp1+(size-image.getWidth()),size*sizeof(fftw_complex));
    memcpy(tmp+size,tmp1,(size-image.getWidth())*sizeof(fftw_complex));
  }
//  fftw_complex *tmp = new fftw_complex[image.getHeight()*(image.getWidth()/2+1)];
//  int size = image.getHeight() / 2 * (image.getWidth()/2+1);
//  memcpy(tmp,s2c+size,size*sizeof(fftw_complex));
//  memcpy(tmp+size,s2c,size*sizeof(fftw_complex));
  auto fftimg = std::make_shared<FitsImage>(image.getName()+"_FFT",image.getWidth(),image.getHeight(),2);
  PixelIterator it2 = fftimg->getPixelIterator();
  fftw_complex* cptr = tmp;
  for (int i=0;i<image.getHeight()*image.getWidth();i++)
  {
    it2[0] = (*cptr)[0];
    it2[1] = (*cptr)[1];
    ++cptr;
    ++it2;
  }
  delete [] tmp;
  delete [] tmp1;
#else
  FitsImage fftimg(image.getName()+"_FFT",image.getWidth()/2+1,image.getHeight(),2);
  PixelIterator it2 = fftimg.getPixelIterator();
  fftw_complex* cptr = s2c;
  for (int i=0;i<fftimg.getHeight()*fftimg.getWidth();i++)
  {
    it2[0] = (*cptr)[0];
    it2[1] = (*cptr)[1];
    ++cptr;
    ++it2;
  }
#endif
  fftw_destroy_plan(f);
  delete [] s2c;
  delete [] in;
  return fftimg;
}
