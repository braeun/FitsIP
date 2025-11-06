/********************************************************************************
 *                                                                              *
 * FitsIP - create a test image with a Moffat distribution                      *
 *                                                                              *
 * modified: 2025-11-01                                                         *
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

#include "moffattestimage.h"
#include "moffattestimagedialog.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/math/mathfunctions.h>

MoffatTestImage::MoffatTestImage():
  dlg(nullptr)
{
}

MoffatTestImage::~MoffatTestImage()
{
}

bool MoffatTestImage::requiresImage() const
{
  return false;
}

bool MoffatTestImage::requiresFileList() const
{
  return false;
}

std::vector<std::shared_ptr<FitsObject>> MoffatTestImage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString MoffatTestImage::getMenuEntry() const
{
  return "Image/Test Images/Moffat...";
}

OpPlugin::ResultType MoffatTestImage::execute(std::shared_ptr<FitsObject> /*image*/, const OpPluginData& data)
{
  if (!dlg)
  {
    dlg = new MoffatTestImageDialog();
  }
  if (dlg->exec())
  {
    img = FitsImage("Gauss",dlg->getWidth(),dlg->getHeight());
    double cx = dlg->getCenterX();
    double cy = dlg->getCenterY();
    double alphax = dlg->getAlphaX();
    double alphay = dlg->getAlphaY();
    double beta = dlg->getBeta();
    PixelIterator it = img.getPixelIterator();
    for (int y=0;y<img.getHeight();y++)
    {
      for (int x=0;x<img.getWidth();x++)
      {
        it[0] = math_functions::moffat(x,y,cx,alphax,cy,alphay,beta);
        ++it;
      }
    }
    emit logOperation("New Image","Created Moffat distribution");
    return OK;
  }
  return CANCELLED;
}


