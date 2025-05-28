/********************************************************************************
 *                                                                              *
 * FitsIP - definition of a filter kernel                                       *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#include "kernel.h"

Kernel::Kernel():
  name(""),
  width(0),
  height(0)
{
}

Kernel::Kernel(const QString& n, const QString& t, uint32_t w, uint32_t h):
  name(n),
  type(t),
  width(w),
  height(h)
{
  for (uint32_t i=0;i<height;i++)
  {
    kern.push_back(std::vector<ValueType>(w,0.0));
  }
}

Kernel::Kernel(const QString& n, const QString& t, uint32_t w, uint32_t h, const std::vector<std::vector<ValueType>>& k):
  name(n),
  type(t),
  width(w),
  height(h),
  kern(k)
{
  normalize();
}

const QString& Kernel::getName() const
{
  return name;
}

const QString& Kernel::getType() const
{
  return type;
}

bool Kernel::isEmpty() const
{
  return height == 0 || width == 0;
}

uint32_t Kernel::getWidth() const
{
  return width;
}

uint32_t Kernel::getHeight() const
{
  return height;
}

void Kernel::normalize()
{
  ValueType sum = 0;
  for (const std::vector<ValueType>& v1 : kern)
  {
    for (ValueType v : v1) sum += v;
  }
  if (sum != 0)
  {
    for (std::vector<ValueType>& v1 : kern)
    {
      for (ValueType& v : v1) v /= sum;
    }
  }
}

const std::vector<std::vector<ValueType>>& Kernel::getKernel() const
{
  return kern;
}

