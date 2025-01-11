/********************************************************************************
 *                                                                              *
 * FitsIP - shrink image                                                        *
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

#include "opshrink.h"
#include "opshrinkdialog.h"
#include <fitsbase/fitsimage.h>

OpShrink::OpShrink():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpShrink");
}

OpShrink::~OpShrink()
{
  if (dlg) dlg->deleteLater();
}

QString OpShrink::getMenuEntry() const
{
return "Image/Shrink...";
}

OpPlugin::ResultType OpShrink::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  if (dlg == nullptr)
  {
    dlg = new OpShrinkDialog();
  }
  if (dlg->exec())
  {
    uint32_t factor = dlg->getFactor();
    profiler.start();
    uint32_t w = image->getImage()->getWidth() / factor;
    uint32_t h = image->getImage()->getHeight() / factor;
    auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getImage()->getDepth());
    img->setMetadata(image->getImage()->getMetadata());
    for (uint32_t d=0;d<image->getImage()->getDepth();d++)
    {
      ValueType* src = image->getImage()->getLayer(d)->getData();
      for (uint32_t y=0;y<h*factor;y++)
      {
        ValueType* dst = img->getLayer(d)->getData() + (y / factor * w);
        for (uint32_t x=0;x<w*factor;x++)
        {
          dst[x/factor] += src[x];
        }
        src += image->getImage()->getWidth();
      }
    }
    image->setImage(img);
    profiler.stop();
    log(image,QString("OpShrink: factor=%1").arg(factor));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}
