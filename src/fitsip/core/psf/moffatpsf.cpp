/********************************************************************************
 *                                                                              *
 * FitsIP - Moffat point-spread-functions                                       *
 *                                                                              *
 * modified: 2025-11-01                                                         *
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

#include "moffatpsf.h"
#include "../math/mathfunctions.h"

MoffatPSF::MoffatPSF():PSF()
{
}

MoffatPSF::~MoffatPSF()
{
}

QString MoffatPSF::getName() const
{
  return "Moffat";
}

ValueType MoffatPSF::value(ValueType x, ValueType y, const std::vector<ValueType>& par) const
{
  return math_functions::moffat(x,y,0,par[0],0,par[1],par[2]);
}

std::vector<QString> MoffatPSF::getParameterNames() const
{
  return std::vector<QString>{"alpha x", "alpha y", "beta"};
}


