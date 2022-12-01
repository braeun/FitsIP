/********************************************************************************
 *                                                                              *
 * FitsIP - calculate the inverse FFT                                           *
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

#include "opinvfft.h"

#include <fitsimage.h>
#include <fftw3.h>

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

bool OpInvFFT::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsImage>> OpInvFFT::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsImage>>{img};
}


QString OpInvFFT::getMenuEntry() const
{
  return "Math/Inverse FFT";
}

OpPlugin::ResultType OpInvFFT::execute(std::shared_ptr<FitsImage> image, QRect aoi)
{
  profiler.start();
  if (aoi.isNull())
    img = invfft(image);
  else
    img = invfft(image->subImage(aoi));
  profiler.stop();
  if (!img) return ERROR;
  logProfiler(img);
  return OK;
}

std::shared_ptr<FitsImage> OpInvFFT::invfft(std::shared_ptr<FitsImage> image)
{
  if (image->getDepth() != 2 || image->preFFTHeight == 0 || image->preFFTWidth == 0)
  {
    setError("Input is not a complex image");
    return std::shared_ptr<FitsImage>();
  }
  fftw_complex* in = new fftw_complex[image->getHeight()*image->getWidth()];
  double *out = new double[image->preFFTHeight*image->preFFTWidth];
  fftw_plan f = fftw_plan_dft_c2r_2d(image->preFFTHeight,image->preFFTWidth,in,out,FFTW_ESTIMATE);
  ConstPixelIterator it = image->getConstPixelIterator();
  fftw_complex* cptr = in;
  for (uint32_t i=0;i<image->getHeight()*image->getWidth();i++)
  {
    (*cptr)[0] = it[0];
    (*cptr)[1] = it[1];
    ++it;
    ++cptr;
  }
  fftw_execute(f);
  auto fftimg = std::make_shared<FitsImage>(image->getName()+"_INVFFT",image->preFFTWidth,image->preFFTHeight,1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = out;
  for (uint32_t i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
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
