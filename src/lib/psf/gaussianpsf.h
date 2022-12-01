/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian shaped point-spread-functions                              *
 *                                                                              *
 * modified: 2022-11-25                                                         *
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

#ifndef GAUSSIANPSF_H
#define GAUSSIANPSF_H

#include "psf.h"

class GaussianPSF: public PSF
{
public:
  GaussianPSF();
  virtual ~GaussianPSF() override;

  virtual QString getName() const override;

  virtual ValueType value(ValueType x, ValueType y, const std::vector<ValueType>& par) const override;

  virtual std::vector<QString> getParameterNames() const override;

};

#endif // GAUSSIANPSF_H
