/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian blur                                                       *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#ifndef OPGAUSSBLUR_H
#define OPGAUSSBLUR_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpGaussBlurDialog;

/**
 * @brief Performs a gaussian blur on the image.
 *
 * Based on the Gaussian Blur plugin from ImageJ, which is in the public domain
 *
 * @author Harald Braeuning
 */
class OpGaussBlur: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpGaussBlur();
  virtual ~OpGaussBlur() override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  void blur(std::shared_ptr<FitsImage> image, ValueType sigmax, ValueType sigmay, ValueType accuracy=0.1);

  /**
   * @brief Creates a 1-dimensional normalized Gaussian kernel with standard deviation sigma
   *  and the running sum over the kernel.
   *
   *  Note: this is one side of the kernel only!
   *  To avoid a step due to the cutoff at a finite value, the near-edge values are
   *  replaced by a 2nd-order polynomial with its minimum=0 at the first out-of-kernel
   *  pixel. Thus, the kernel function has a smooth 1st derivative in spite of finite
   *  length.
   *
   * @param sigma     Standard deviation, i.e. radius of decay to 1/sqrt(e), in pixels.
   * @param accuracy  Relative accuracy; for best results below 0.01 when processing
   *                  8-bit images. For short or ValueType images, values of 1e-3 to 1e-4
   *                  are better (but increase the kernel size and thereby the
   *                  processing time). Edge smoothing will fail with very poor
   *                  accuracy (above approx. 0.02)
   * @param maxRadius Maximum radius of the kernel: Limits kernel size in case of
   *                  large sigma, should be set to image width or height. For small
   *                  values of maxRadius, the kernel returned may have a larger
   *                  radius, however.
   * @return          A 2*n array. Array[0][n] is the kernel, decaying towards zero,
   *                  which would be reached at kernel.length (unless kernel size is
   *                  limited by maxRadius). Array[1][n] holds the sum over all kernel
   *                  values larger n, including non-calculated values in case the kernel
   *                  size is limited by <code>maxRadius</code>.
   */
  static std::vector<std::vector<ValueType>> makeGaussianKernel(ValueType sigma, ValueType accuracy, int maxRadius);

private:
  void blur(std::shared_ptr<FitsImage> image);
  /**
   * Blurs an image in X direction.
   */
  void blurX(ValueType* pixel, int32_t width, int32_t height);
  void blurY(ValueType* pixel, int32_t width, int32_t height);
  void convolveLine(ValueType* input, int32_t length, ValueType* pixels, const std::vector<std::vector<ValueType>>& kernel);

  void downscaleLine(ValueType* pixels, ValueType* cache, const std::vector<ValueType>& kernel, int reduceBy, int unscaled0, int length, int newLength);
  std::vector<ValueType> makeDownscaleKernel(int unitLength);
  void upscaleLine(ValueType* cache, ValueType* pixels, int npixels, const std::vector<ValueType>& kernel, int reduceBy, int unscaled0);
  std::vector<ValueType> makeUpscaleKernel(int unitLength);

  uint32_t reduceByX;
  uint32_t reduceByY;
  std::vector<std::vector<ValueType>> gaussKernelX;
  std::vector<std::vector<ValueType>> gaussKernelY;
  std::vector<ValueType> downscaleKernelX;
  std::vector<ValueType> downscaleKernelY;
  std::vector<ValueType> upscaleKernelX;
  std::vector<ValueType> upscaleKernelY;
  OpGaussBlurDialog* dlg;

  static const int UPSCALE_K_RADIUS;         // number of pixels to add for upscaling
  static const ValueType MIN_DOWNSCALED_SIGMA; // minimum standard deviation in the downscaled image


};

#endif // OPGAUSSBLUR_H
