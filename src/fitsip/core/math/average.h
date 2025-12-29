/********************************************************************************
 *                                                                              *
 * FitsIP - average of a list of values                                         *
 *                                                                              *
 * modified: 2025-12-29                                                         *
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

#ifndef AVERAGE_H
#define AVERAGE_H

#include <vector>

class Average
{
public:
  Average();
  Average(const std::vector<double>& list);

  inline void add(double v);

  int getN() const;

  double getMean() const;

  double getVariance() const;

  inline Average& operator+=(double v);

private:
  int n;
  double sum;
  double sum2;
};

void Average::add(double v)
{
  ++n;
  sum += v;
  sum2 += v * v;
}

Average& Average::operator+=(double v)
{
  add(v);
  return *this;
}

#endif // AVERAGE_H
