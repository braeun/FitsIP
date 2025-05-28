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

#ifndef AVERAGE_H
#define AVERAGE_H

#include <vector>

class Average
{
public:
  Average();
  Average(const std::vector<double>& list);

  inline void add(double d);

  int getN() const;

  double getMean() const;

  double getVariance() const;

private:
  int n;
  double sum;
  double sum2;
};

void Average::add(double d)
{
  ++n;
  sum += d;
  sum2 += d * d;
}



#endif // AVERAGE_H
