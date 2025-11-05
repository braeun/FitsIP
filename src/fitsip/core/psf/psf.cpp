/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for point-spread-functions                       *
 *                                                                              *
 * modified: 2025-11-05                                                         *
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

ValueType PSF::value(ValueType x, ValueType y, const std::vector<ValueType>& par) const
{
  return 0;
}

FitsImage PSF::createPSF(int w, int h, const std::vector<ValueType>& par) const
{
  FitsImage img(getName(),w,h);
  ValueType sum = 0;
  for (int y=0;y<img.getHeight();y++)
  {
    ValueType yv = y < h/2 ? y : h - y;
    PixelIterator it = img.getPixelIterator(0,y);
    for (int x=0;x<img.getWidth();x++)
    {
      ValueType xv = x < w/2 ? x : w - x;
      it[0] = value(xv,yv,par);
      sum += it[0];
      ++it;
    }
  }
  if (sum != 0) img /= sum;
  return img;
}

FitsImage PSF::createPSFForDisplay(int w, int h, const std::vector<ValueType>& par) const
{
  FitsImage img(getName(),w,h);
  PixelIterator it = img.getPixelIterator();
  for (int y=0;y<img.getHeight();y++)
  {
    ValueType yv = y - img.getHeight() / 2.0;
    for (int x=0;x<img.getWidth();x++)
    {
      ValueType xv = x - img.getWidth() / 2.0;
      it[0] = value(xv,yv,par);
      ++it;
    }
  }
  return img;
}

std::vector<QString> PSF::getParameterNames() const
{
  return std::vector<QString>();
}

bool PSF::isFixedSize() const
{
  return false;
}

int PSF::getWidth() const
{
  return 0;
}

int PSF::getHeight() const
{
  return 0;
}

