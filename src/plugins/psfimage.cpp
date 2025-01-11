/********************************************************************************
 *                                                                              *
 * FitsIP - create image from point spread function                             *
 *                                                                              *
 * modified: 2022-11-25                                                         *
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

#include "psfimage.h"
#include <fitsbase/fitsimage.h>
#include <fitsbase/dialogs/psfdialog.h>
#include <fitsbase/psf/psf.h>
#include <fitsbase/psf/psffactory.h>
#include <fitsbase/math/mathfunctions.h>

PSFImage::PSFImage()
{
}

PSFImage::~PSFImage()
{
}

bool PSFImage::requiresImage() const
{
  return false;
}

bool PSFImage::requiresFileList() const
{
  return false;
}

bool PSFImage::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsObject>> PSFImage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}


QString PSFImage::getMenuEntry() const
{
  return "Image/PSF...";
}

OpPlugin::ResultType PSFImage::execute(std::shared_ptr<FitsObject> /*image*/, QRect /*selection*/, const PreviewOptions& opt)
{
  PSFDialog d;
  if (d.exec())
  {
    const PSF* psf = PSFFactory::getInstance()->getPSF(d.getFunction());
    if (psf)
    {
      img = psf->createPSFForDisplay(d.getWidth(),d.getHeight(),d.getParameters());
      emit logOperation("New Image","Created PSF: "+psf->getName());
      return OK;
    }
    else
    {
      setError("Internal Error: PSF '"+d.getFunction()+"' not found!");
      return ERROR;
    }
  }
  return CANCELLED;
}

