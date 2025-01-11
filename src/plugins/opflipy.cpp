/********************************************************************************
 *                                                                              *
 * FitsIP - flip image vertically                                               *
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

#include "opflipy.h"
#include <fitsbase/fitsimage.h>

OpFlipY::OpFlipY()
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpFlipY");
}

OpFlipY::~OpFlipY()
{
}

QString OpFlipY::getMenuEntry() const
{
  return "Image/Flip Y";
}

QIcon OpFlipY::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/object-flip-vertical.png");
}

OpPlugin::ResultType OpFlipY::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  profiler.start();
  for (uint32_t y=0;y<image->getImage()->getHeight()/2;y++)
  {
    PixelIterator it1 = image->getImage()->getPixelIterator(0,y);
    PixelIterator it2 = image->getImage()->getPixelIterator(0,image->getImage()->getHeight()-1-y);
    for (uint32_t x=0;x<=image->getImage()->getWidth();x++)
    {
      for (uint32_t d=0;d<image->getImage()->getDepth();d++)
      {
        ValueType v = it1[d];
        it1[d] = it2[d];
        it2[d] = v;
      }
      ++it1;
      ++it2;
    }
  }
  profiler.stop();
  log(image,"flipped in Y");
  logProfiler(image);
  return OK;
}
