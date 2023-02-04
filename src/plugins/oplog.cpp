/********************************************************************************
 *                                                                              *
 * FitsIP - scale image by logarithm                                            *
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

#include "oplog.h"
#include <fitsimage.h>
#include <cmath>

OpLog::OpLog()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpLog");
}

OpLog::~OpLog()
{
}

QString OpLog::getMenuEntry() const
{
  return "Math/Log";
}

QIcon OpLog::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/oplog.png");
}

OpPlugin::ResultType OpLog::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
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
        p[0] = log10(p[0]);
      ++p;
    }
  }
  else
  {
    while (p.hasNext())
    {
      ValueType val = p.getAbs();
      ValueType nval = log10(val);
      for (uint32_t d=0;d<image->getDepth();d++) p[d] = p[d] * nval / val;
      ++p;
    }
  }
  profiler.stop();
  log(image,"Log of image");
  logProfiler(image);
  return OK;
}
