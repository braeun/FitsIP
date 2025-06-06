/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for point-spread-functions                       *
 *                                                                              *
 * modified: 2025-02-28                                                         *
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

#ifndef PSF_H
#define PSF_H

#include "../fitsimage.h"
#include <QString>
#include <vector>

/**
 * @brief Pure virtual base class for point-spread-functions.
 *
 * When convolving, one often has a spatial-domain kernel with the origin
 * in the middle (as for example in this recent question). In this case,
 * one must shift the origin to the top-left before computing the DFT.
 */
class PSF
{
public:
  PSF();
  virtual ~PSF();

  virtual QString getName() const = 0;

  virtual ValueType value(ValueType x, ValueType y, const std::vector<ValueType>& par) const;

  /**
   * @brief Create a fits image of the point-spread-function suitable for (de)convolution.
   *
   * In this case the center of the point-spread-function is situated at the
   * corners of the image. In addition, the total intensity of the image is
   * normalized to 1.
   * @param w width of the resulting image
   * @param h height of the resulting image
   * @param par parameters for the psf
   * @return the image
   */
  virtual std::shared_ptr<FitsImage> createPSF(int w, int h, const std::vector<ValueType>& par) const;

  /**
   * @brief Create a fits image with the point-spread-function suitable for display.
   *
   * In this case the point-spread-function is centered in the middle of the image.
   * @param w width of the resulting image (might be ignored)
   * @param h height of the resulting image (might be ignored)
   * @param par parameters for the psf
   * @return the image
   */
  virtual std::shared_ptr<FitsImage> createPSFForDisplay(int w, int h, const std::vector<ValueType>& par) const;

  /**
   * @brief Return a vector with all parameter names.
   * @return vector of parameter names
   */
  virtual std::vector<QString> getParameterNames() const;

  /**
   * @brief Return if the PSF has a fixed size (typical for image based PSF).
   * @return true if the PSF is a fixed size
   */
  virtual bool isFixedSize() const;

  /**
   * @brief Return the width of the PSF.
   *
   * Only PSFs with a fixed size are expected to return their width. Other PSFs
   * should return 0. The value returned here can be used for example in
   * FFT deconvolution to pad the image and PSF to a combined size.
   * @return the width
   */
  virtual int getWidth() const;

  /**
   * @brief Return the height of the PSF.
   *
   * Only PSFs with a fixed size are expected to return their height. Other PSFs
   * should return 0. The value returned here can be used for example in
   * FFT deconvolution to pad the image and PSF to a combined size.
   * @return the height
   */
  virtual int getHeight() const;

};

#endif // PSF_H
