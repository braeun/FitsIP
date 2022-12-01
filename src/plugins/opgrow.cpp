/********************************************************************************
 *                                                                              *
 * FitsIP - enlarge image                                                       *
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

#include "opgrow.h"
#include "opgrowdialog.h"
#include <fitsimage.h>

OpGrow::OpGrow():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpGrow");
}

OpGrow::~OpGrow()
{
if (dlg) dlg->deleteLater();
}

QString OpGrow::getMenuEntry() const
{
return "Image/Grow...";
}

OpPlugin::ResultType OpGrow::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  if (dlg == nullptr)
  {
    dlg = new OpGrowDialog();
  }
  if (dlg->exec())
  {
    uint32_t factor = dlg->getFactor();
    bool bilinear = dlg->isBilinearInterpolation();
    profiler.start();
    image = grow(image,factor,bilinear);
    profiler.stop();
    log(image,QString("Grow: factor=%1 %2").arg(factor).arg(bilinear?"bilinear":"nearest neighbor"));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

std::shared_ptr<FitsImage> OpGrow::grow(std::shared_ptr<FitsImage> image, uint32_t factor, bool bilinear)
{
  uint32_t w = image->getWidth() * factor;
  uint32_t h = image->getHeight() * factor;
  auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getDepth());
  img->setMetadata(image->getMetadata());
  PixelIterator it = img->getPixelIterator();
  if (bilinear)
  {
    for (uint32_t y=0;y<h;y++)
    {
      for (uint32_t x=0;x<w;x++)
      {
        uint32_t xp = x / factor;
        uint32_t yp = y / factor;
        for (uint32_t d=0;d<image->getDepth();d++)
        {
          double A = image->getConstPixelIterator(xp,yp)[d];
          double B = A;
          if (xp+1 < image->getWidth()) B = image->getConstPixelIterator(xp+1,yp)[d];
          double C = A;
          if (yp+1 < image->getHeight()) C = image->getConstPixelIterator(xp,yp+1)[d];
          double D = A;
          if (yp+1 < image->getHeight() && xp+1 < image->getWidth()) D = image->getConstPixelIterator(xp+1,yp+1)[d];
          double a = static_cast<double>(x) / static_cast<double>(factor) - xp;
          double b = static_cast<double>(y) / static_cast<double>(factor) - yp;
          double v = (a - 1) * (b - 1) * A + a * (1 - b) * B + (1 - a) * b * C + a * b * D;
          it[d] = v;
          ++it;
        }
      }
    }
  }
  else
  {
    for (uint32_t y=0;y<h;y++)
    {
      for (uint32_t x=0;x<w;x++)
      {
        uint32_t xp = x / factor;
        uint32_t yp = y / factor;
        ConstPixelIterator it2 = image->getConstPixelIterator(xp,yp);
        for (uint32_t d=0;d<image->getDepth();d++)
        {
          it[d] = it2[d];
        }
        ++it;
      }
    }
  }
  return img;
}
