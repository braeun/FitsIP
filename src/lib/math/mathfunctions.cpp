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

#include "mathfunctions.h"
#include <cmath>

ValueType MathFunctions::gaussian(ValueType x, ValueType a, ValueType c, ValueType s)
{
  return a * exp(-(x-c)*(x-c)/2/s/s);
}

ValueType MathFunctions::gaussian(ValueType x, ValueType y, ValueType a, ValueType cx, ValueType sx, ValueType cy, ValueType sy)
{
  return a * exp(-((x-cx)*(x-cx)/2/sx/sx+(y-cy)*(y-cy)/2/sy/sy));
}










MathFunctions::MathFunctions()
{
}
