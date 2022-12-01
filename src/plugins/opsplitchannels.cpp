/********************************************************************************
 *                                                                              *
 * FitsIP - split channels of a multilayer image                                *
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

#include "opsplitchannels.h"
#include <fitsimage.h>

OpSplitChannels::OpSplitChannels()
{
  profiler = SimpleProfiler("OpSplitChannels");
}

OpSplitChannels::~OpSplitChannels()
{
}

bool OpSplitChannels::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsImage>> OpSplitChannels::getCreatedImages() const
{
  return images;
}


QString OpSplitChannels::getMenuEntry() const
{
  return "Color/Split";
}

OpPlugin::ResultType OpSplitChannels::execute(std::shared_ptr<FitsImage> image, QRect /*aoi*/)
{
  if (image->getDepth() != 3)
  {
    setError("Not a color image");
    return ERROR;
  }
  profiler.start();
  images = split(image);
  profiler.stop();
  logProfiler(image);
  return OK;
}

std::vector<std::shared_ptr<FitsImage>> OpSplitChannels::split(std::shared_ptr<FitsImage> image)
{
  std::vector<std::shared_ptr<FitsImage>> list{
    std::make_shared<FitsImage>(image->getName()+"_R",image->getWidth(),image->getHeight(),1),
    std::make_shared<FitsImage>(image->getName()+"_G",image->getWidth(),image->getHeight(),1),
    std::make_shared<FitsImage>(image->getName()+"_B",image->getWidth(),image->getHeight(),1)
  };
  list[0]->setMetadata(image->getMetadata());
  list[1]->setMetadata(image->getMetadata());
  list[2]->setMetadata(image->getMetadata());
  ConstPixelIterator src = image->getConstPixelIterator();
  PixelIterator r = list[0]->getPixelIterator();
  PixelIterator g = list[1]->getPixelIterator();
  PixelIterator b = list[2]->getPixelIterator();
  while (src.hasNext())
  {
    r[0] = src[0];
    g[0] = src[1];
    b[0] = src[2];
    ++src;
    ++r;
    ++g;
    ++b;
  }
  return list;
}

