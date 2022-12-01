/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for point-spread-functions                       *
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

#include "psf.h"

PSF::PSF()
{
}

PSF::~PSF()
{
}

std::shared_ptr<FitsImage> PSF::createPSF(uint32_t w, uint32_t h, const std::vector<ValueType>& par) const
{
  auto img = std::make_shared<FitsImage>(getName(),w,h);
  ValueType sum = 0;
  for (uint32_t y=0;y<img->getHeight();y++)
  {
    ValueType yv = y < h/2 ? y : h - y;
    PixelIterator it = img->getPixelIterator(0,y);
    for (uint32_t x=0;x<img->getWidth();x++)
    {
      ValueType xv = x < w/2 ? x : w - x;
      it[0] = value(xv,yv,par);
      sum += it[0];
      ++it;
    }
  }
  *img /= sum;
  return img;
}

std::shared_ptr<FitsImage> PSF::createPSFForDisplay(uint32_t w, uint32_t h, const std::vector<ValueType>& par) const
{
  auto img = std::make_shared<FitsImage>(getName(),w,h);
  for (uint32_t y=0;y<img->getHeight();y++)
  {
    ValueType yv = y - img->getHeight() / 2;
    PixelIterator it = img->getPixelIterator(0,y);
    for (uint32_t x=0;x<img->getWidth();x++)
    {
      ValueType xv = x - img->getWidth() / 2;
      it[0] = value(xv,yv,par);
      ++it;
    }
  }
  return img;
}


