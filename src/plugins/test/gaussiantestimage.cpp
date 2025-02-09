/********************************************************************************
 *                                                                              *
 * FitsIP - create gaussian test image                                          *
 *                                                                              *
 * modified: 2023-02-03                                                         *
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

#include "gaussiantestimage.h"
#include "gaussiantestimagedialog.h"
#include <fitsbase/fitsimage.h>
#include <fitsbase/math/mathfunctions.h>

GaussianTestImage::GaussianTestImage():
  dlg(nullptr)
{
}

GaussianTestImage::~GaussianTestImage()
{
}

bool GaussianTestImage::requiresImage() const
{
  return false;
}

bool GaussianTestImage::requiresFileList() const
{
  return false;
}

bool GaussianTestImage::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsObject>> GaussianTestImage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString GaussianTestImage::getMenuEntry() const
{
  return "Image/Test Images/Gaussian...";
}

OpPlugin::ResultType GaussianTestImage::execute(std::shared_ptr<FitsObject> /*image*/, QRect /*selection*/, const PreviewOptions& opt)
{
  if (!dlg)
  {
    dlg = new GaussianTestImageDialog();
  }
  if (dlg->exec())
  {
    img = std::make_shared<FitsImage>("Gauss",dlg->getWidth(),dlg->getHeight());
    double a = dlg->getAmplitude();
    double cx = dlg->getCenterX();
    double sx = dlg->getSigmaX();
    double cy = dlg->getCenterY();
    double sy = dlg->getSigmaY();
    PixelIterator it = img->getPixelIterator();
    for (int y=0;y<img->getHeight();y++)
    {
      for (int x=0;x<img->getWidth();x++)
      {
        it[0] = MathFunctions::gaussian(x,y,a,cx,sx,cy,sy);
        ++it;
      }
    }
    emit logOperation("New Image","Created gaussian");
    return OK;
  }
  return CANCELLED;
}

