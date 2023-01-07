/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for point-spread-functions                       *
 *                                                                              *
 * modified: 2023-01-07                                                         *
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
 */
class PSF
{
public:
  PSF();
  virtual ~PSF();

  virtual QString getName() const = 0;

  virtual ValueType value(ValueType x, ValueType y, const std::vector<ValueType>& par) const = 0;

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
  virtual std::shared_ptr<FitsImage> createPSF(uint32_t w, uint32_t h, const std::vector<ValueType>& par) const;

  /**
   * @brief Create a fits image with the point-spread-function suitable for display.
   *
   * In this case the point-spread-function is centered in the middle of the image.
   * @param w width of the resulting image
   * @param h height of the resulting image
   * @param par parameters for the psf
   * @return the image
   */
  virtual std::shared_ptr<FitsImage> createPSFForDisplay(uint32_t w, uint32_t h, const std::vector<ValueType>& par) const;

  virtual std::vector<QString> getParameterNames() const = 0;

};

#endif // PSF_H
