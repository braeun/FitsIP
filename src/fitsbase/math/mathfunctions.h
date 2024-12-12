/********************************************************************************
 *                                                                              *
 * FitsIP - often used mathematical functions                                   *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

class MathFunctions
{
public:

  /**
   * @brief Calculate the value of a one-dimensional gaussian function.
   * @param x the x value
   * @param ampl the amplitude
   * @param center the center
   * @param sigma the signam
   * @return the value of the gaussian
   */
  static ValueType gaussian(ValueType x, ValueType ampl, ValueType center, ValueType sigma);

  /**
   * @brief Calculate the value of a two-dimensional gaussian function.
   * @param x the x value
   * @param y the y value
   * @param ampl the amplitude
   * @param centerx the center in x
   * @param sigmax the sigma in x
   * @param centery the center in y
   * @param sigmay the signam in y
   * @return the value of the gaussian
   */
  static ValueType gaussian(ValueType x, ValueType y, ValueType ampl, ValueType centerx, ValueType sigmax, ValueType centery, ValueType sigmay);

private:
  MathFunctions();
};

#endif // MATHFUNCTIONS_H
