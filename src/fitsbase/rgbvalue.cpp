/********************************************************************************
 *                                                                              *
 * FitsIP - class to represent na RGB value                                     *
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

#include "rgbvalue.h"
#include <math.h>

/*
 * Return the color value according to the index i. This allows easy use of RGBValue in loops.
 */
ValueType RGBValue::operator[](int i) const
{
  switch (i)
  {
    case RGB_SUM:
      return r + g + b;
    case RGB_RED:
      return r;
    case RGB_GREEN:
      return g;
    case RGB_BLUE:
      return b;
  }
  return 0;
}




ValueType RGBValue::gray(ValueType r, ValueType g, ValueType b)
{
  return (r * 11 + g * 16 + b * 5) / 32;
}


void RGBValue::getHSI(ValueType *h, ValueType *s, ValueType *i)
{
  *h = atan2(r,b);
  *i = sqrt(r*r+g*g+b*b);
  *s = acos(g/(*i));
}

void RGBValue::setHSI(ValueType h, ValueType s, ValueType i)
{
  g = cos(s) * i;
  ValueType t = tan(h);
  b = sqrt((i*i-g*g)/(1+t*t));
  r = b * t;
}

void RGBValue::setUpperLimit(ValueType d)
{
  if (r > d) r = d;
  if (g > d) g = d;
  if (b > d) b = d;
}

void RGBValue::setLowerLimit(ValueType d)
{
  if (r < d) r = d;
  if (g < d) g = d;
  if (b < d) b = d;
}
