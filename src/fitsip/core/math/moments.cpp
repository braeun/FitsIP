/********************************************************************************
 *                                                                              *
 * FitsIP - moments of a distribution                                           *
 *                                                                              *
 * modified: 2025-01-12                                                         *
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

#include "moments.h"
#include <cmath>

Moments::Moments(const QVector<QPointF>& dist)
{
  calculate(dist);
}

double Moments::getIntegral() const
{
  return integral;
}

double Moments::getCenter() const
{
  return center;
}

double Moments::getVariance() const
{
  return variance;
}

double Moments::getStandardDeviation() const
{
  return std::sqrt(variance);
}

double Moments::getSkewness() const
{
  return skewness;
}

double Moments::getKurtosis() const
{
  return kurtosis;
}





void Moments::calculate(const QVector<QPointF>& dist)
{
  integral = 0;
  center = 0;
  variance = 0;
  skewness = 0;
  kurtosis = 0;
  for (QPointF p : dist)
  {
    integral += std::abs(p.y());
    center += p.x() * std::abs(p.y());
  }
  if (integral > 0)
  {
    center /= integral;
    for (QPointF p : dist)
    {
      double v = p.x() - center;
      variance += v * v * std::abs(p.y());
      skewness += v * v * v * std::abs(p.y());
      kurtosis += v * v * v * v * std::abs(p.y());
    }
    variance /= integral;
    double sigma = std::sqrt(variance);
    skewness /= (variance * sigma);
    kurtosis /= (variance * variance);
  }
}
