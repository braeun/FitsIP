/******************************************************************************
 *                                                                            *
 * hmath - linear regression to points                                        *
 *                                                                            *
 * last modified: 2023-07-23                                                  *
 *                                                                            *
 ******************************************************************************
 * Copyright (C) Harald Braeuning                                             *
 ******************************************************************************
 * This file is part of hmath.                                                *
 *                                                                            *
 * hmath is free software; you can redistribute it and/or  modify it under    *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation; either version 3 of the License, or (at your option)  *
 *  any later version.                                                        *
 *                                                                            *
 * hmath is distributed in the hope that it will be useful, but WITHOUT ANY   *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or          *
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for  *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with hmath. If not, see <http://www.gnu.org/licenses/>.                    *
 *****************************************************************************/

#include "linearregression.h"
#include <cmath>
#include <stdexcept>

LinearRegression::LinearRegression():
    n(0),
    xbar(0),ybar(0),
    sdx(0),sdy(0),
    RSS(0),
    rms(0),
    seslope(0),seintercept(0),
    slope(0),intercept(0),
    SSreg(0),F(0)
{
}

LinearRegression::~LinearRegression()
{
}

void LinearRegression::execute(const std::vector<double> &x, const std::vector<double> &y)
{
  if (x.size() != y.size()) throw std::invalid_argument("sizes of x and y vectors do not match");
  n = std::min(x.size(),y.size());
  if (n < 2) throw std::invalid_argument("not enough data points (at least 2)");
  xbar = ybar = 0.0;
  for (size_t i=0;i<n;i++)
  {
    xbar += x[i];
    ybar += y[i];
  }
  xbar = xbar / n;
  ybar = ybar / n;
  /* sums of squares */
  double SXX = 0.0;
  double SYY = 0.0;
  double SXY = 0.0;
  for (size_t i=0;i<n;i++)
  {
    SXX += (x[i] - xbar) * (x[i] - xbar);
    SYY += (y[i] - ybar) * (y[i] - ybar);
    SXY += (x[i] - xbar) * (y[i] - ybar);
  }
  sdx = std::sqrt(SXX / (n - 1));
  sdy = std::sqrt(SYY / (n - 1));
  if (sdx == 0.0 || sdy == 0.0) throw std::runtime_error("linear regression is undertermined: sdx == 0 || sdy == 0");
  sxy = SXY / (n - 1);
  rxy = sxy / (sdx * sdy);
  slope = SXY / SXX;
  intercept = ybar - slope * xbar;

  RSS = SYY - slope * SXY;

  rms = RSS / (n - 2);
  //sereg = sqrt(rms);
  seintercept = sqrt(rms * (1.0 / n + xbar * xbar / SXX));
  seslope = sqrt(rms / SXX);
  SSreg = SYY - RSS;
  F = SSreg / rms;
  //R2 = SSreg / SYY;
}

