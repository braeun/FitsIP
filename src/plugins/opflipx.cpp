/********************************************************************************
 *                                                                              *
 * FitsIP - flip image horizontally                                             *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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

#include "opflipx.h"
#include <fitsimage.h>

OpFlipX::OpFlipX()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpFlipX");
}

OpFlipX::~OpFlipX()
{
}

QString OpFlipX::getMenuEntry() const
{
  return "Image/Flip X";
}

QIcon OpFlipX::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/object-flip-horizontal.png");
}

OpPlugin::ResultType OpFlipX::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  profiler.start();
  for (uint32_t y=0;y<image->getHeight();y++)
  {
    PixelIterator it1 = image->getPixelIterator(0,y);
    PixelIterator it2 = image->getPixelIterator(image->getWidth()-1,y);
    for (uint32_t x=0;x<=image->getWidth()/2;x++)
    {
      for (uint32_t d=0;d<image->getDepth();d++)
      {
        ValueType v = it1[d];
        it1[d] = it2[d];
        it2[d] = v;
      }
      ++it1;
      --it2;
    }
  }
  profiler.stop();
  log(image,"flipped in X");
  logProfiler(image);
  return OK;
}
