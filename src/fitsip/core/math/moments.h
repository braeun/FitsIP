/********************************************************************************
 *                                                                              *
 * FitsIP - moments of a distribution                                           *
 *                                                                              *
 * modified: 2025-06-07                                                         *
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

#ifndef MOMENTS_H
#define MOMENTS_H

#include <QPointF>
#include <QVector>

class Moments
{
public:
  Moments(const QVector<QPointF>& dist, double threshold=0);

  double getIntegral() const;

  double getCenter() const;

  double getVariance() const;

  double getStandardDeviation() const;

  double getSkewness() const;

  double getKurtosis() const;

protected:
  void calculate(const QVector<QPointF>& dist, double threshold);

private:
  double integral;
  double center;
  double variance;
  double skewness;
  double kurtosis;
};

#endif // MOMENTS_H
