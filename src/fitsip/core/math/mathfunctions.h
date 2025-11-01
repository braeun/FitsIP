/********************************************************************************
 *                                                                              *
 * FitsIP - often used mathematical functions                                   *
 *                                                                              *
 * modified: 2025-05-24                                                         *
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

#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include "../fitstypes.h"

namespace math_functions
{
  /**
   * @brief Calculate the value of a one-dimensional gaussian function.
   * @param x the x value
   * @param ampl the amplitude
   * @param center the center
   * @param sigma the signam
   * @return the value of the gaussian
   */
  extern ValueType gaussian(ValueType x, ValueType ampl, ValueType center, ValueType sigma);

  /**
   * @brief Calculate the value of a two-dimensional gaussian function.
   * @param x the x value
   * @param y the y value
   * @param ampl the amplitude
   * @param centerx the center in x
   * @param sigmax the sigma in x
   * @param centery the center in y
   * @param sigmay the sigma in y
   * @return the value of the gaussian
   */
  extern ValueType gaussian(ValueType x, ValueType y, ValueType ampl, ValueType centerx, ValueType sigmax, ValueType centery, ValueType sigmay);

  /**
   * @brief Calculate the value of a two-dimensional box function.
   *
   * A box function is zero outside the box [centerx-width/2,centerx+width/2],[centery-height/2,centery+height/2].
   * Inside the box its value is given by amplitude.
   * @param x the x value
   * @param y the y value
   * @param ampl the amplitude
   * @param centerx the center in x
   * @param width the width in x
   * @param centery the center in y
   * @param height the height in y
   * @return the value
   */
  extern ValueType box(ValueType x, ValueType y, ValueType ampl, ValueType centerx, ValueType width, ValueType centery, ValueType height);

  /**
   * @brief Calculate the value of a radial (one-dimensional) Moffat distribution
   * @param r the radius
   * @param center the center of the distribution
   * @param alpha alpha parameter (width)
   * @param beta beta parameter (exponent)
   * @return the value
   */
  extern ValueType moffat(ValueType r, ValueType center, ValueType alpha, ValueType beta);

  /**
   * @brief Calculate the value of a two-dimensional Moffat distribution
   * @param x the x value
   * @param y the y value
   * @param centerx the center in x
   * @param alphax alpha parameter (width) in x
   * @param centery the center in y
   * @param alphay alpha parameter (width) in y
   * @param beta beta parameter (exponent)
   * @return the value
   */
  extern ValueType moffat(ValueType x, ValueType y, ValueType centerx, ValueType alphax, ValueType centery, ValueType alphay, ValueType beta);

}

#endif // MATHFUNCTIONS_H
