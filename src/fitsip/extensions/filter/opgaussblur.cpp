/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian blur                                                       *
 *                                                                              *
 * modified: 2025-02-07                                                         *
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

#include "opgaussblur.h"
#include "opgaussblurdialog.h"
#include <fitsip/core/fitsimage.h>
#include <cmath>
#include <climits>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

const int OpGaussBlur::UPSCALE_K_RADIUS = 2;         // number of pixels to add for upscaling
const ValueType OpGaussBlur::MIN_DOWNSCALED_SIGMA = 4.; // minimum standard deviation in the downscaled image

OpGaussBlur::OpGaussBlur():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpGaussBlur");
}

OpGaussBlur::~OpGaussBlur()
{
}

QString OpGaussBlur::getMenuEntry() const
{
  return "Filter/Gaussian Blur...";
}

#ifdef USE_PYTHON
void OpGaussBlur::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("gauss_blur",[this](std::shared_ptr<FitsObject> obj, ValueType sigmax, ValueType sigmay, ValueType accuracy){
    blur(obj->getImage(),sigmax,sigmay,accuracy);
    obj->getImage()->log(QString::asprintf("Gaussian blur: sigmax=%f  sigmay=%f  accuracy=%f",sigmax,sigmay,accuracy));
    return OK;
  },
  "Blur an image by a gaussian",py::arg("obj"),py::arg("sigmax"),py::arg("sigmay"),py::arg("accuracy"));
}
#endif

OpPlugin::ResultType OpGaussBlur::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new OpGaussBlurDialog();
  dlg->setSourceImage(image->getImage(),data.aoi,data.previewOptions);
  if (dlg->exec())
  {
    ValueType sigmax = dlg->getSigma();
    ValueType sigmay = dlg->getSigma();
    ValueType accuracy = 0.1;
    profiler.start();
    blur(image->getImage(),sigmax,sigmay,accuracy);
    profiler.stop();
    log(image,QString::asprintf("Gaussian blur: sigmax=%f  sigmay=%f  accuracy=%f",sigmax,sigmay,accuracy));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}



void OpGaussBlur::blur(std::shared_ptr<FitsImage> image, ValueType sigmax, ValueType sigmay, ValueType accuracy) const
{
  Data data;
  if (sigmax > 2 * MIN_DOWNSCALED_SIGMA + 0.5)
  {
    /* large radius (sigma): scale down, then convolve, then scale up */
    data.reduceByX = static_cast<int>(floor(fabs(sigmax)/MIN_DOWNSCALED_SIGMA)); //downscale by this factor
    if (data.reduceByX > image->getWidth()) data.reduceByX = image->getWidth();
    /*
     * Downscale gives std devation sigma = 1/sqrt(3); upscale gives sigma =
     * 1/2. (in downscaled pixels)
     */
    /* All sigma^2 values add to full sigma^2 */
    ValueType sigmaGauss = sqrt(sigmax * sigmax / (data.reduceByX * data.reduceByX) - 1. / 3. - 1. / 4.);
    int maxLength = (image->getWidth() + data.reduceByX - 1) / data.reduceByX + 2 * (UPSCALE_K_RADIUS + 1); //downscaled line can't be longer
    data.gaussKernelX = makeGaussianKernel(sigmaGauss,accuracy,maxLength);
    data.downscaleKernelX = makeDownscaleKernel(data.reduceByX);
    data.upscaleKernelX = makeUpscaleKernel(data.reduceByX);
  }
  else
  {
    data.reduceByX = 1;
    data.downscaleKernelX.clear();
    data.upscaleKernelX.clear();
    data.gaussKernelX = makeGaussianKernel(sigmax,accuracy,image->getWidth());
  }
  if (sigmay > 2 * MIN_DOWNSCALED_SIGMA + 0.5)
  {
    /* large radius (sigma): scale down, then convolve, then scale up */
    data.reduceByY = static_cast<int>(floor(fabs(sigmay) / MIN_DOWNSCALED_SIGMA)); //downscale by this factor
    if (data.reduceByY > image->getHeight()) data.reduceByY = image->getHeight();
    /*
     * Downscale gives std devation sigma = 1/sqrt(3); upscale gives sigma =
     * 1/2. (in downscaled pixels)
     */
    /* All sigma^2 values add to full sigma^2 */
    ValueType sigmaGauss = sqrt(sigmay * sigmay / (data.reduceByY * data.reduceByY) - 1. / 3. - 1. / 4.);
    int maxLength = (image->getHeight() + data.reduceByY - 1) / data.reduceByY + 2 * (UPSCALE_K_RADIUS + 1); //downscaled line can't be longer
    data.gaussKernelY = makeGaussianKernel(sigmaGauss,accuracy,maxLength);
    data.downscaleKernelY = makeDownscaleKernel(data.reduceByY);
    data.upscaleKernelY = makeUpscaleKernel(data.reduceByY);
  }
  else
  {
    data.reduceByY = 1;
    data.downscaleKernelY.clear();
    data.upscaleKernelY.clear();
    data.gaussKernelY = makeGaussianKernel(sigmax,accuracy,image->getHeight());
  }
  for (int d=0;d<image->getDepth();d++)
  {
    blurX(image->getLayer(d)->getData(),image->getWidth(),image->getHeight(),data);
    blurY(image->getLayer(d)->getData(),image->getWidth(),image->getHeight(),data);
  }
}

std::vector<std::vector<ValueType>> OpGaussBlur::makeGaussianKernel(ValueType sigma, ValueType accuracy, int maxRadius)
{
  int32_t kRadius = static_cast<int32_t>(ceil(sigma * sqrt(-2 * ::log(accuracy)))) + 1;
  if (maxRadius < 50) maxRadius = 50;  // too small maxRadius would result in inaccurate sum.
  if (kRadius > maxRadius) kRadius = maxRadius;
  std::vector<std::vector<ValueType>> kernel;
  kernel.push_back(std::vector<ValueType>(kRadius,0));
  kernel.push_back(std::vector<ValueType>(kRadius,0));
  for (int32_t i=0;i<kRadius;i++)               // Gaussian function
  {
    kernel[0][i] = exp(-0.5 * i * i / sigma / sigma);
  }
  if (kRadius < maxRadius && kRadius > 3)
  {   // edge correction
    ValueType sqrtSlope = std::numeric_limits<ValueType>::max();
    int r = kRadius;
    while (r > kRadius / 2)
    {
      r--;
      ValueType a = sqrt(kernel[0][r]) / (kRadius - r);
      if (a < sqrtSlope)
        sqrtSlope = a;
      else
        break;
    }
    for (int r1 = r + 2;r1 < kRadius;r1++)
    {
      kernel[0][r1] = (kRadius - r1) * (kRadius - r1) * sqrtSlope * sqrtSlope;
    }
  }
  ValueType sum;                                 // sum over all kernel elements for normalization
  if (kRadius < maxRadius)
  {
    sum = kernel[0][0];
    for (int i = 1;i < kRadius;i++)
    {
      sum += 2 * kernel[0][i];
    }
  }
  else
    sum = sigma * sqrt(2 * M_PI);

  ValueType rsum = 0.5 + 0.5 * kernel[0][0] / sum;
  for (int i = 0;i < kRadius;i++)
  {
    ValueType v = (kernel[0][i] / sum);
    kernel[0][i] = v;
    rsum -= v;
    kernel[1][i] = rsum;
  }
  return kernel;
}







/**
 * Blurs an image in X direction.
 */
void OpGaussBlur::blurX(ValueType* pixel, int32_t width, int32_t height, const Data& data) const
{
  int length = width;   //number of points per line (line can be a row or column)
  if (data.reduceByX > 1)
  {
    int newLength = (length + data.reduceByX - 1) / data.reduceByX + 2 * (UPSCALE_K_RADIUS + 1);
    int unscaled0 = -(UPSCALE_K_RADIUS + 1) * data.reduceByX; //input point corresponding to cache index 0
    //IJ.log("reduce="+reduceBy+", newLength="+newLength+", unscaled0="+unscaled0+", sigmaG="+(ValueType)sigmaGauss+", kRadius="+gaussKernel[0].length);
    ValueType* cache1 = new ValueType[newLength];  //holds data after downscaling
    ValueType* cache2 = new ValueType[newLength];  //holds data after convolution
    for (int y=0;y<height;y++)
    {
      downscaleLine(pixel+y*width,cache1,data.downscaleKernelX,data.reduceByX,unscaled0,length,newLength);
      convolveLine(cache1,newLength,cache2,data.gaussKernelX);
      upscaleLine(cache2,pixel+y*width,length,data.upscaleKernelX,data.reduceByX,unscaled0);
    }
    delete [] cache1;
    delete [] cache2;
  }
  else
  {
    ValueType* cache = new ValueType[length];          //input for convolution, hopefully in CPU cache
    for (int y=0;y<height;y++)
    {
      memcpy(cache,pixel+y*width,length*sizeof(ValueType));
      convolveLine(cache,length,pixel+y*width,data.gaussKernelX);
    }
    delete [] cache;
  }
}

/**
 * Blurs an image in Y direction.
 */
void OpGaussBlur::blurY(ValueType* pixel, int32_t width, int32_t height, const Data& data) const
{
  int length = height;   //number of points per line (line can be a row or column)
  ValueType* tmp = new ValueType[length];
  if (data.reduceByY > 1)
  {
    int newLength = (length + data.reduceByY - 1) / data.reduceByY + 2 * (UPSCALE_K_RADIUS + 1);
    int unscaled0 = -(UPSCALE_K_RADIUS + 1) * data.reduceByY; //input point corresponding to cache index 0
    //IJ.log("reduce="+reduceBy+", newLength="+newLength+", unscaled0="+unscaled0+", sigmaG="+(ValueType)sigmaGauss+", kRadius="+gaussKernel[0].length);
    ValueType* cache1 = new ValueType[newLength];  //holds data after downscaling
    ValueType* cache2 = new ValueType[newLength];  //holds data after convolution
    for (int x=0;x<width;x++)
    {
      for (int y=0;y<length;y++) tmp[y] = pixel[y*width+x];
      downscaleLine(tmp,cache1,data.downscaleKernelY,data.reduceByY,unscaled0,length,newLength);
      convolveLine(cache1,newLength,cache2,data.gaussKernelY);
      upscaleLine(cache2,tmp,length,data.upscaleKernelY,data.reduceByY,unscaled0);
      for (int y=0;y<length;y++) pixel[y*width+x] = tmp[y];
    }
    delete [] cache1;
    delete [] cache2;
  }
  else
  {
    ValueType* cache = new ValueType[length];          //input for convolution, hopefully in CPU cache
    for (int x=0;x<width;x++)
    {
      for (int y=0;y<length;y++) tmp[y] = pixel[y*width+x];
      memcpy(cache,tmp,length*sizeof(ValueType));
      convolveLine(cache,length,tmp,data.gaussKernelY);
      for (int y=0;y<length;y++) pixel[y*width+x] = tmp[y];
    }
    delete [] cache;
  }
  delete [] tmp;
}


/**
 * Convolve a line with a symmetric kernel and write to a separate array,
 * possibly the pixels array of a ValueTypeProcessor (as a row or column or part
 * thereof)
 *
 * @param input Input array containing the line
 * @param pixels ValueType array for output, can be the pixels of a ValueTypeProcessor
 * @param kernel "One-sided" kernel array, kernel[0][n] must contain the
 * kernel itself, kernel[1][n] must contain the running sum over all kernel
 * elements from kernel[0][n+1] to the periphery. The kernel must be
 * normalized, i.e. sum(kernel[0][n]) = 1 where n runs from the kernel
 * periphery (last element) to 0 and back. Normalization should include all
 * kernel points, also these not calculated because they are not needed.
 */
void OpGaussBlur::convolveLine(ValueType* input, int32_t length, ValueType* pixels, const std::vector<std::vector<ValueType>>& kernel) const
{
  ValueType first = input[0];                 //out-of-edge pixels are replaced by nearest edge pixels
  ValueType last = input[length - 1];
  const std::vector<ValueType>& kern = kernel[0];               //the kernel itself
  ValueType kern0 = kern[0];
  const std::vector<ValueType>& kernSum = kernel[1];            //the running sum over the kernel
  int kRadius = kern.size();
  int firstPart = kRadius < length ? kRadius : length;
  int p = 0;
  int i = 0;
  for (;i < firstPart;i++,p++)
  {  //while the sum would include pixels < 0
    ValueType result = input[i] * kern0;
    result += kernSum[i] * first;
    if (i + kRadius > length) result += kernSum[length - i - 1] * last;
    for (int k = 1;k < kRadius;k++)
    {
      ValueType v = 0;
      if (i - k >= 0) v += input[i-k];
      if (i + k < length) v += input[i+k];
      result += kern[k] * v;
    }
    pixels[p] = result;
  }
  int iEndInside = length - kRadius;
  for (;i < iEndInside;i++,p++)
  {   //while only pixels within the line are be addressed (the easy case)
    ValueType result = input[i] * kern0;
    for (int k = 1;k < kRadius;k++)
    {
      result += kern[k] * (input[i-k] + input[i+k]);
    }
    pixels[p] = result;
  }
  for (;i<length;i++,p++)
  {    //while the sum would include pixels >= length
    ValueType result = input[i] * kern0;
    if (i < kRadius) result += kernSum[i] * first;
    if (i + kRadius >= length) result += kernSum[length-i-1] * last;
    for (int k = 1;k < kRadius;k++)
    {
      ValueType v = 0;
      if (i - k >= 0) v += input[i-k];
      if (i + k < length) v += input[i+k];
      result += kern[k] * v;
    }
    pixels[p] = result;
  }
}

/**
 * Scale a line (row or column of a ValueTypeProcessor or part thereof) down by a
 * factor
 * <code>reduceBy</code> and write the result into
 * <code>cache</code>. Input line pixel #
 * <code>unscaled0</code> will correspond to output line pixel # 0.
 * <code>unscaled0</code> may be negative. Out-of-line pixels of the input are
 * replaced by the edge pixels.
 */
void OpGaussBlur::downscaleLine(ValueType* pixels, ValueType* cache, const std::vector<ValueType>& kernel, int reduceBy, int unscaled0, int length, int newLength) const
{
  ValueType first = pixels[0];
  ValueType last = pixels[length - 1];
  int xin = unscaled0 - reduceBy / 2;
  int p = xin;
  for (int xout = 0;xout < newLength;xout++)
  {
    ValueType v = 0;
    for (int x = 0;x < reduceBy;x++,xin++,p++)
    {
      v += kernel[x] * ((xin - reduceBy < 0) ? first : ((xin - reduceBy >= length) ? last : pixels[p - reduceBy]));
      v += kernel[x + reduceBy] * ((xin < 0) ? first : ((xin >= length) ? last : pixels[p]));
      v += kernel[x + 2 * reduceBy] * ((xin + reduceBy < 0) ? first : ((xin + reduceBy >= length) ? last : pixels[p + reduceBy]));
    }
    cache[xout] = v;
  }
}

/*
 * Create a kernel for downscaling. The kernel function preserves norm and 1st
 * moment (i.e., position) and has fixed 2nd moment, (in contrast to linear
 * interpolation). In scaled space, the length of the kernel runs from -1.5 to
 * +1.5, and the standard deviation is 1/2. Array index corresponding to the
 * kernel center is unitLength*3/2
 */
std::vector<ValueType> OpGaussBlur::makeDownscaleKernel(int unitLength) const
{
  int mid = unitLength * 3 / 2;
  std::vector<ValueType> kernel(3*unitLength,0);
  for (int i=0;i<=unitLength/2;i++)
  {
    ValueType x = i / static_cast<ValueType>(unitLength);
    ValueType v = (0.75 - x * x) / unitLength;
    kernel[mid - i] = v;
    kernel[mid + i] = v;
  }
  for (int i=unitLength/2+1;i<(unitLength*3+1)/ 2;i++)
  {
    ValueType x = i / static_cast<ValueType>(unitLength);
    ValueType v = (0.125 + 0.5 * (x - 1) * (x - 2)) / unitLength;
    kernel[mid - i] = v;
    kernel[mid + i] = v;
  }
  return kernel;
}

/**
 * Scale a line up by factor
 * <code>reduceBy</code> and write as a row or column (or part thereof) to the
 * pixels array of a ValueTypeProcessor.
 */
void OpGaussBlur::upscaleLine(ValueType* cache, ValueType* pixels, int npixels, const std::vector<ValueType>& kernel, int reduceBy, int unscaled0) const
{
  int p = 0;
  for (int xout=0;xout<npixels;xout++,p++)
  {
    int xin = (xout - unscaled0 + reduceBy - 1) / reduceBy; //the corresponding point in the cache (if exact) or the one above
    int x = reduceBy - 1 - (xout - unscaled0 + reduceBy - 1) % reduceBy;
    pixels[p] = cache[xin - 2] * kernel[x]
            + cache[xin - 1] * kernel[x + reduceBy]
            + cache[xin] * kernel[x + 2 * reduceBy]
            + cache[xin + 1] * kernel[x + 3 * reduceBy];
  }
}

/**
 * Create a kernel for upscaling. The kernel function is a convolution of four
 * unit squares, i.e., four uniform kernels with value +1 from -0.5 to +0.5
 * (in downscaled coordinates). The second derivative of this kernel is
 * smooth, the third is not. Its standard deviation is 1/sqrt(3) in downscaled
 * cordinates. The kernel runs from [-2 to +2[, corresponding to array index 0
 * ... 4*unitLength (whereby the last point is not in the array any more).
 */
std::vector<ValueType> OpGaussBlur::makeUpscaleKernel(int unitLength) const
{
  std::vector<ValueType> kernel(4*unitLength,0);
  int mid = 2 * unitLength;
  kernel[0] = 0;
  for (int32_t i=0;i<unitLength;i++)
  {
    ValueType x = i / static_cast<ValueType>(unitLength);
    ValueType v = (2. / 3. - x * x * (1 - 0.5 * x));
    kernel[mid + i] = v;
    kernel[mid - i] = v;
  }
  for (int i = unitLength;i < 2 * unitLength;i++)
  {
    ValueType x = i / static_cast<ValueType>(unitLength);
    ValueType v = (2. - x) * (2. - x) * (2. - x) / 6.;
    kernel[mid + i] = v;
    kernel[mid - i] = v;
  }
  return kernel;
}

