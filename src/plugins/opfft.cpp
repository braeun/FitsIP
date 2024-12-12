/********************************************************************************
 *                                                                              *
 * FitsIP - calculate the FFT of an image                                       *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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
#include <fitsbase/fitsimage.h>
#include <fftw3.h>

/**
 * @note: FFTW requires arrays in row-major (or C) format with the last
 * dimension changing the fastest. For the images, this is the width (x)
 * dimension. Thus for calling FFTW routines, the dimensions must be passed
 * in the order height, width.
 */

OpFFT::OpFFT()
{
  profiler = SimpleProfiler("OpFFT");
}

OpFFT::~OpFFT()
{
}

bool OpFFT::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsImage>> OpFFT::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsImage>>{img};
}


QString OpFFT::getMenuEntry() const
{
  return "Math/FFT";
}

OpPlugin::ResultType OpFFT::execute(std::shared_ptr<FitsImage> image, QRect aoi)
{
  profiler.start();
  if (aoi.isNull())
    img = fft(*image);
  else
    img = fft(*image->subImage(aoi));
  profiler.stop();
  logProfiler(img);
  return OK;
}

std::shared_ptr<FitsImage> OpFFT::fft(const FitsImage &image)
{
  fftw_complex *s2c = new fftw_complex[image.getHeight()*(image.getWidth()/2+1)];
  double *in = new double[image.getHeight()*image.getWidth()];
  fftw_plan f = fftw_plan_dft_r2c_2d(image.getHeight(),image.getWidth(),in,s2c,FFTW_ESTIMATE);
  ConstPixelIterator it = image.getConstPixelIterator();
  double* ptr = in;
  for (uint32_t i=0;i<image.getHeight()*image.getWidth();i++)
  {
    *ptr++ = it.getAbs();
    ++it;
  }
  fftw_execute(f);
#if 0
  fftw_complex *tmp1 = new fftw_complex[image.getHeight()*image.getWidth()];
  for (uint32_t y=0;y<image.getHeight();y++)
  {
    fftw_complex* ptr = tmp1 + y * image.getWidth();
    fftw_complex* sptr = s2c + y * (image.getWidth()/2+1);
    if (image.getWidth()%2 == 0)
    {
      memcpy(&ptr[image.getWidth()/2-1],sptr,(image.getWidth()/2+1)*sizeof(fftw_complex));
      sptr++;
      for (uint32_t x=1;x<image.getWidth()/2;x++)
      {
        memcpy(&ptr[image.getWidth()/2-1-x],sptr,sizeof(fftw_complex));
        sptr++;
      }
    }
    else
    {
      memcpy(&ptr[image.getWidth()/2],sptr,(image.getWidth()/2+1)*sizeof(fftw_complex));
      sptr++;
      for (uint32_t x=1;x<image.getWidth()/2+1;x++)
      {
        memcpy(&ptr[image.getWidth()/2-x],sptr,sizeof(fftw_complex));
        sptr++;
      }
    }
  }
  fftw_complex *tmp = new fftw_complex[image.getHeight()*image.getWidth()];
  if (image.getHeight()%2 == 0)
  {
    uint32_t size = image.getHeight() / 2 * image.getWidth();
    memcpy(tmp,tmp1+size,size*sizeof(fftw_complex));
    memcpy(tmp+size,tmp1,size*sizeof(fftw_complex));
  }
  else
  {
    uint32_t size = (image.getHeight() / 2 + 1) * image.getWidth();
    memcpy(tmp,tmp1+(size-image.getWidth()),size*sizeof(fftw_complex));
    memcpy(tmp+size,tmp1,(size-image.getWidth())*sizeof(fftw_complex));
  }
//  fftw_complex *tmp = new fftw_complex[image.getHeight()*(image.getWidth()/2+1)];
//  uint32_t size = image.getHeight() / 2 * (image.getWidth()/2+1);
//  memcpy(tmp,s2c+size,size*sizeof(fftw_complex));
//  memcpy(tmp+size,s2c,size*sizeof(fftw_complex));
  auto fftimg = std::make_shared<FitsImage>(image.getName()+"_FFT",image.getWidth(),image.getHeight(),2);
  PixelIterator it2 = fftimg->getPixelIterator();
  fftw_complex* cptr = tmp;
  for (uint32_t i=0;i<image.getHeight()*image.getWidth();i++)
  {
    it2[0] = (*cptr)[0];
    it2[1] = (*cptr)[1];
    ++cptr;
    ++it2;
  }
  delete [] tmp;
  delete [] tmp1;
#else
  auto fftimg = std::make_shared<FitsImage>(image.getName()+"_FFT",image.getWidth()/2+1,image.getHeight(),2);
  fftimg->preFFTHeight = image.getHeight();
  fftimg->preFFTWidth = image.getWidth();
  PixelIterator it2 = fftimg->getPixelIterator();
  fftw_complex* cptr = s2c;
  for (uint32_t i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
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
