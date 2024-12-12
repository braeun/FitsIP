/********************************************************************************
 *                                                                              *
 * FitsIP - convert image to gray scale image                                   *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#include "optogray.h"
#include <fitsbase/fitsimage.h>

OpToGray::OpToGray()
{
  profiler = SimpleProfiler("OpToGray");
}

OpToGray::~OpToGray()
{
}

QString OpToGray::getMenuEntry() const
{
  return "Color/To Gray";
}

OpPlugin::ResultType OpToGray::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  if (image->getDepth() != 3)
  {
    setError("Not a color image");
    return ERROR;
  }
  profiler.start();
  *image = *toGray(image);
  profiler.stop();
  log(image,"Converted to gray image");
  logProfiler(image);
  return OK;
}

std::shared_ptr<FitsImage>  OpToGray::toGray(std::shared_ptr<FitsImage> image)
{
  return image->toGray();
}
