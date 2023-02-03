/********************************************************************************
 *                                                                              *
 * FitsIP - cosine bell shaped point-spread-function                            *
 *                                                                              *
 * modified: 2023-02-03                                                         *
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

#include "cosinebellpsf.h"
#include <cmath>

CosineBellPSF::CosineBellPSF():PSF()
{
}

CosineBellPSF::~CosineBellPSF()
{
}

QString CosineBellPSF::getName() const
{
  return "Cosine Bell";
}

ValueType CosineBellPSF::value(ValueType x, ValueType y, const std::vector<ValueType>& par) const
{
  if (par[1] <= par[0]) return 0;
  ValueType r = sqrt(x*x+y*y);
  if (r <= par[0]) return 1;
  if (r >= par[1]) return 0;
  ValueType R = par[1] - par[0];
  ValueType q = (r - par[0]) / R;
  return sqrt(1 - q * q);
}

std::vector<QString> CosineBellPSF::getParameterNames() const
{
  return std::vector<QString>{"Inner Radius", "Outer Radius"};
}

