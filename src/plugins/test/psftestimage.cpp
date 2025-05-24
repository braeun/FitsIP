/********************************************************************************
 *                                                                              *
 * FitsIP - create a test image from a PSF                                      *
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

#include "psftestimage.h"
#include "psftestimagedialog.h"
#include <fitsbase/fitsimage.h>
#include <fitsbase/psf/psffactory.h>

PSFTestImage::PSFTestImage():
  dlg(nullptr)
{
}

PSFTestImage::~PSFTestImage()
{
}

bool PSFTestImage::requiresImage() const
{
  return false;
}

bool PSFTestImage::requiresFileList() const
{
  return false;
}

bool PSFTestImage::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsObject>> PSFTestImage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString PSFTestImage::getMenuEntry() const
{
  return "Image/Test Images/PSF...";
}

OpPlugin::ResultType PSFTestImage::execute(std::shared_ptr<FitsObject> /*image*/, const OpPluginData& data)
{
  if (!dlg)
  {
    dlg = new PSFTestImageDialog();
  }
  dlg->updatePSFList();
  if (dlg->exec())
  {
    img = std::make_shared<FitsImage>(dlg->getFunction(),dlg->getWidth(),dlg->getHeight());
    ValueType a = dlg->getAmplitude();
    std::vector<ValueType> par = dlg->getParameters();
    auto psf = PSFFactory::getInstance()->getPSF(dlg->getFunction());
    if (psf)
    {
      img = psf->createPSFForDisplay(dlg->getWidth(),dlg->getHeight(),par);
      *img *= a;
      emit logOperation("New Image","Created test image for "+dlg->getFunction());
      return OK;
    }
    else
    {
      setError("Internal Error: PSF '"+dlg->getFunction()+"' not found!");
      return ERROR;
    }
  }
  return CANCELLED;
}

