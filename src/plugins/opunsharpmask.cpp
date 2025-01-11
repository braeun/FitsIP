/********************************************************************************
 *                                                                              *
 * FitsIP - unsharp masking                                                     *
 *                                                                              *
 * modified: 2025-01-10                                                         *
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

#include "opunsharpmask.h"
#include "opunsharpmaskdialog.h"
#include "opgaussblur.h"
#include <fitsbase/fitsimage.h>

OpUnsharpMask::OpUnsharpMask():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpUnsharpMask");
}

OpUnsharpMask::~OpUnsharpMask()
{
}

QString OpUnsharpMask::getMenuEntry() const
{
  return "Filter/Unsharp Mask...";
}

OpPlugin::ResultType OpUnsharpMask::execute(std::shared_ptr<FitsObject> image, QRect selection, const PreviewOptions& opt)
{
  if (!dlg) dlg = new OpUnsharpMaskDialog();
  dlg->setSourceImage(image->getImage(),selection,opt);
  if (dlg->exec())
  {
    ValueType sigma = dlg->getSigma();
    ValueType strength = dlg->getStrength();
    profiler.start();
    unsharpmask(image->getImage(),sigma,strength);
    profiler.stop();
    log(image,QString::asprintf("Unsharp mask: sigma=%f  strength=%f",sigma,strength));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpUnsharpMask::unsharpmask(std::shared_ptr<FitsImage> image, ValueType sigma, ValueType strength)
{
  OpGaussBlur blur;
  auto blurred = std::make_shared<FitsImage>(*image);
  blur.blur(blurred,sigma,sigma);
  for (uint32_t d=0;d<image->getDepth();d++)
  {
    ValueType *p1 = image->getLayer(d)->getData();
    ValueType *p2 = blurred->getLayer(d)->getData();
    uint32_t n = image->getWidth() * image->getHeight();
    while (n-- > 0)
    {
      *p1 = (1 + strength) * *p1 - strength * *p2;
      ++p1;
      ++p2;
    }
  }
}


