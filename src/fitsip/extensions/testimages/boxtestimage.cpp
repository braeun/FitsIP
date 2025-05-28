/********************************************************************************
 *                                                                              *
 * FitsIP - create box test image                                               *
 *                                                                              *
 * modified: 2025-05-24                                                         *
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

#include "boxtestimage.h"
#include "boxtestimagedialog.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/math/mathfunctions.h>

BoxTestImage::BoxTestImage():
  dlg(nullptr)
{
}

BoxTestImage::~BoxTestImage()
{
}

bool BoxTestImage::requiresImage() const
{
  return false;
}

bool BoxTestImage::requiresFileList() const
{
  return false;
}

bool BoxTestImage::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsObject>> BoxTestImage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString BoxTestImage::getMenuEntry() const
{
  return "Image/Test Images/Box...";
}

OpPlugin::ResultType BoxTestImage::execute(std::shared_ptr<FitsObject> /*image*/, const OpPluginData& /*data*/)
{
  if (!dlg)
  {
    dlg = new BoxTestImageDialog();
  }
  if (dlg->exec())
  {
    img = std::make_shared<FitsImage>("Box",dlg->getWidth(),dlg->getHeight());
    double a = dlg->getAmplitude();
    int cx = dlg->getCenterX();
    int sx = dlg->getBoxWidth();
    int cy = dlg->getCenterY();
    int sy = dlg->getBoxHeight();
    PixelIterator it = img->getPixelIterator();
    for (int y=0;y<img->getHeight();y++)
    {
      for (int x=0;x<img->getWidth();x++)
      {
        it[0] = math_functions::box(x,y,a,cx,sx,cy,sy);
        ++it;
      }
    }
    emit logOperation("New Image","Created box");
    return OK;
  }
  return CANCELLED;
}

