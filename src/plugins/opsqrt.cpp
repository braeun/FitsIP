/********************************************************************************
 *                                                                              *
 * FitsIP - scale image intensity by square root                                *
 *                                                                              *
 * modified: 2023-02-04                                                         *
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

#include "opsqrt.h"
#include <fitsbase/fitsimage.h>
#include <cmath>
#include <iostream>

OpSqrt::OpSqrt()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpSqrt");
}

OpSqrt::~OpSqrt()
{
}

QString OpSqrt::getMenuEntry() const
{
  return "Math/Sqrt";
}

QIcon OpSqrt::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/opsqrt.png");
}

OpPlugin::ResultType OpSqrt::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  profiler.start();
  PixelIterator p = image->getPixelIterator();
  if (image->getDepth() == 1)
  {
    while (p.hasNext())
    {
      if (p[0] <= 0)
        p[0] = 0;
      else
        p[0] = sqrt(p[0]);
      ++p;
    }
  }
  else
  {
    while (p.hasNext())
    {
      ValueType val = p.getAbs();
      ValueType nval = sqrt(val);
      for (uint32_t d=0;d<image->getDepth();d++) p[d] = p[d] * nval / val;
      ++p;
    }
  }
  profiler.stop();
  log(image,"Square Root of image");
  logProfiler(image);
  return OK;
}
