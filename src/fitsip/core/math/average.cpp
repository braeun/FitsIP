/********************************************************************************
 *                                                                              *
 * FitsIP - average of a list of values                                         *
 *                                                                              *
 * modified: 2025-02-15                                                         *
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

#include "average.h"

Average::Average():
  n(0),
  sum(0),
  sum2(0)
{
}

Average::Average(const std::vector<double>& list):
  n(0),
  sum(0),
  sum2(0)
{
  for (double d : list)
  {
    ++n;
    sum += d;
    sum2 += d * d;
  }
}

int Average::getN() const
{
  return n;
}

double Average::getMean() const
{
  if (n == 0) return 0;
  return sum / n;
}

double Average::getVariance() const
{
  if (n < 2) return 0;
  return (n * sum2 - sum * sum) / (n * static_cast<double>(n-1));
}



