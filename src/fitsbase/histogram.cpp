/********************************************************************************
 *                                                                              *
 * FitsIP - intensity histogram                                                 *
 *                                                                              *
 * modified: 2025-01-10                                                         *
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

#include "histogram.h"
#include "fitsimage.h"
#include <algorithm>
#include <cstring>
#include <climits>
#include <QDebug>

#define DEFAULT_BINS 1024 /* number of bins in histogram */

Histogram::Histogram():
  bin(DEFAULT_BINS),
  min(0.0),
  max(4096)
{
}

Histogram::Histogram(ValueType min, ValueType max, int32_t bin):
  bin(bin),
  min(min),
  max(max)
{
}


/*
 * Dtor
 */
Histogram::~Histogram(void)
{
}

/*
 * Stream output operator for histogram
 */
std::ostream &operator<<(std::ostream &s, const Histogram &h)
{
  for (int n=0;n<h.bin;n++)
  {
    ValueType v = static_cast<ValueType>(n) * (h.max - h.min) / h.bin + h.min;
    s << v;
    for (int32_t i=0;i<4;i++) s << " " << h.data[i][n];
    s << std::endl;
  }
  return s;
}

/*
 * Clear histogram
 */
void Histogram::clear(void)
{
  for (size_t i=0;i<4;i++)
  {
    data[i].clear();
  }
}

void Histogram::build(FitsImage* img)
{
  for (size_t i=0;i<4;i++)
  {
    data[i].assign(bin,0);
  }
  sum = img->getWidth() * img->getHeight();
  bool rgb = img->getDepth() == 3;
  ValueType ma = -std::numeric_limits<ValueType>::max();
  ValueType mi = std::numeric_limits<ValueType>::max();
  ConstPixelIterator p = img->getConstPixelIterator();
  while (p.hasNext())
  {
    mi = std::min(mi,p.min());
    ma = std::max(ma,p.max());
    ++p;
  }
  min = mi;
  max = ma;
  p = img->getConstPixelIterator();
  while (p.hasNext())
  {
    if (rgb)
    {
      inc(p.getRGB().gray());
      inc(p.getRGB());
    }
    else
    {
      inc(p.getAbs());
    }
    ++p;
  }
}

bool Histogram::isRGB() const
{
  return !data[1].empty();
}

int Histogram::getSum() const
{
  return sum;
}

int Histogram::getSum(int32_t maxbin) const
{
  int64_t sum = 0;
  const int *d = data[0].data();
  for (int i=0;i<std::min(bin,maxbin+1);i++) sum += *d++;
  return sum;
}

ValueType Histogram::getFillLevel(double p) const
{
  int n = static_cast<int32_t>(p*sum);
  int b = 0;
  const int *d = data[0].data();
  while (b < bin && n > 0)
  {
    n -= *d++;
    ++b;
  }
  return getX(b-1);
}

AverageResult Histogram::getAverage(double p) const
{
  int cnt = static_cast<int>(p*sum);
  int b = 0;
  const int *d = data[0].data();
  int64_t n = 0;
  double sum = 0;
  double sum2 = 0;
  while (b < bin && cnt > 0)
  {
    n += *d;
    double x = getX(b);
    sum += *d * x;
    sum2 += *d * x * x;
    cnt -= *d;
    ++d;
    ++b;
  }
  if (cnt < 0)
  {
    --b;
    --d;
    double x = getX(b);
    n += cnt;
    sum += cnt * x;
    sum2 += cnt * x * x;
  }
  if (n == 0)
  {
    return AverageResult{0,0.0,0.0,0.0};
  }
  double mean = sum / n;
  if (n == 1) return AverageResult{n,mean,0.0,0.0};
  double sigma = sqrt((n*sum2-sum*sum)/(n*(n-1)));
  double error = sigma / sqrt(n);
  return AverageResult{n,mean,sigma,error};
}
