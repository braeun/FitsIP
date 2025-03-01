/********************************************************************************
 *                                                                              *
 * FitsIP - cosine bell shaped point-spread-function                            *
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

#ifndef COSINEBELLPSF_H
#define COSINEBELLPSF_H

#include "psf.h"

class CosineBellPSF: public PSF
{
public:
  CosineBellPSF();
  virtual ~CosineBellPSF() override;

  virtual QString getName() const override;

  virtual ValueType value(ValueType x, ValueType y, const std::vector<ValueType>& par) const override;

  virtual std::vector<QString> getParameterNames() const override;

};

#endif // COSINEBELLPSF_H
