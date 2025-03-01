/********************************************************************************
 *                                                                              *
 * FitsIP - image based point-spread-function                                   *
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

#ifndef IMAGEPSF_H
#define IMAGEPSF_H

#include "psf.h"
#include "../fitsimage.h"

class ImagePSF: public PSF
{
public:
  ImagePSF(QString filename);
  virtual ~ImagePSF();

  virtual QString getName() const override;

  virtual void init() override;

  virtual std::shared_ptr<FitsImage> createPSF(int w, int h, const std::vector<ValueType>& par) const override;

  virtual std::shared_ptr<FitsImage> createPSFForDisplay(int w, int h, const std::vector<ValueType>& par) const override;

private:
  QString filename;
  std::shared_ptr<FitsImage> img;
};

#endif // IMAGEPSF_H
