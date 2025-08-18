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

#ifndef LINEARREGRESSION_H
#define LINEARREGRESSION_H

#include <cstddef>
#include <cstdint>
#include <vector>

class LinearRegression
{
public:
  LinearRegression();
  virtual ~LinearRegression();

  size_t getNrSamples() const { return n; }
  double getCorrelationCoefficient() const { return rxy; }
  double getSlope() const { return slope; }
  double getSlopeError() const { return seslope; }
  double getIntercept() const { return intercept; }
  double getInterceptError() const { return seintercept; }

  double getMeanX() const { return xbar; }
  double getStdDevX() const { return sdx; }
  double getMeanY() const { return ybar; }
  double getStdDevY() const { return sdy; }

  void execute(const std::vector<double> &x, const std::vector<double> &y);

private:
  size_t n;
  double xbar, ybar;		/* sample means */
  double sdx, sdy;		/* sample standard deviations */
  double sxy, rxy;		/* sample covariance and sample correlation */
  double RSS;			/* residual sum of squares */
  double rms;			/* residual mean square */
  //double sereg;		/* standard error of regression */
  double seslope, seintercept;
  double slope, intercept;	/* */
  double SSreg, F;//, R2;
};

#endif // LINEARREGRESSION_H
