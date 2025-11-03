/********************************************************************************
 *                                                                              *
 * FitsIP - test image with a ruler                                             *
 *                                                                              *
 * modified: 2025-11-03                                                         *
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

#include "rulerwhiteonblacktestimage.h"
#include <fitsip/core/io/iofactory.h>

static const char* image_file = ":/testimages/images/ruler.512_white_on_black.png";

RulerWhiteOnBlackTestImage::RulerWhiteOnBlackTestImage()
{
}

RulerWhiteOnBlackTestImage::~RulerWhiteOnBlackTestImage()
{
}

bool RulerWhiteOnBlackTestImage::requiresImage() const
{
  return false;
}

bool RulerWhiteOnBlackTestImage::requiresFileList() const
{
  return false;
}

std::vector<std::shared_ptr<FitsObject>> RulerWhiteOnBlackTestImage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{obj};
}


QString RulerWhiteOnBlackTestImage::getMenuEntry() const
{
  return "Image/Test Images/Ruler (white on black)";
}

OpPlugin::ResultType RulerWhiteOnBlackTestImage::execute(std::shared_ptr<FitsObject> /*image*/, const OpPluginData& /*data*/)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(image_file);
  if (handler)
  {
    try
    {
      obj = handler->read(image_file).front();
      return OK;
    }
    catch (std::exception& ex)
    {
      setError("Internal Error: "+QString(ex.what()));
      return ERROR;
    }
  }
  setError("Internal Error: No handler found");
  return ERROR;
}
