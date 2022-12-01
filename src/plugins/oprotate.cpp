/********************************************************************************
 *                                                                              *
 * FitsIP - rotate images                                                       *
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

#include "oprotate.h"
#include "oprotatedialog.h"
#include <fitsimage.h>
#include <cmath>
#include <limits>

OpRotate::OpRotate():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpRotate");
}

OpRotate::~OpRotate()
{
  if (dlg) dlg->deleteLater();
}

QString OpRotate::getMenuEntry() const
{
  return "Image/Rotate...";
}

OpPlugin::ResultType OpRotate::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  if (dlg == nullptr)
  {
    dlg = new OpRotateDialog();
  }
  if (dlg->exec())
  {
    profiler.start();
    if (dlg->isRotate90CW())
    {
      rotate90cw(image);
      profiler.stop();
      log(image,"OpRotate: 90deg cw");
    }
    else if (dlg->isRotate90CCW())
    {
      rotate90ccw(image);
      profiler.stop();
      log(image,"OpRotate: 90deg ccw");
    }
    else
    {
      rotate(image,dlg->getAngle(),false);
      profiler.stop();
      log(image,QString("OpRotate: %1deg").arg(static_cast<ValueType>(dlg->getAngle())));
    }
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpRotate::rotate90cw(std::shared_ptr<FitsImage> image)
{
  FitsImage img(image->getName(),image->getHeight(),image->getWidth(),image->getDepth());
  img.setMetadata(image->getMetadata());
  ConstPixelIterator it = image->getConstPixelIterator();
  for (uint32_t y=0;y<image->getHeight();y++)
  {
    PixelIterator it2 = img.getPixelIterator(image->getHeight()-y-1,0);
    for (uint32_t x=0;x<image->getWidth();x++)
    {
      for (uint32_t d=0;d<image->getDepth();d++) it2[d] = it[d];
//      img.getPixelIterator(image->getHeight()-y-1,x).setRGB(it.getRGB());
      it2 += img.getWidth();
      ++it;
    }
  }
  *image = img;
}

void OpRotate::rotate90ccw(std::shared_ptr<FitsImage> image)
{
  FitsImage img(image->getName(),image->getHeight(),image->getWidth(),image->getDepth());
  img.setMetadata(image->getMetadata());
  ConstPixelIterator it = image->getConstPixelIterator();
  for (uint32_t y=0;y<image->getHeight();y++)
  {
    PixelIterator it2 = img.getPixelIterator(y,image->getWidth()-1);
    for (uint32_t x=0;x<image->getWidth();x++)
    {
      for (uint32_t d=0;d<image->getDepth();d++) it2[d] = it[d];
//      img.getPixelIterator(y,image->getWidth()-x-1).setRGB(it.getRGB());
      it2 -= img.getWidth();
      ++it;
    }
  }
  *image = img;
}

void OpRotate::rotate(std::shared_ptr<FitsImage> image, ValueType angle, bool crop)
{
  if (fabs(angle) < 0.001f) return;
  int32_t wt = static_cast<int32_t>(image->getWidth());
  int32_t ht = static_cast<int32_t>(image->getHeight());
  int32_t xc = wt / 2;
  int32_t yc = ht / 2;
  ValueType ca = cos(angle*M_PI/180.0);
  ValueType sa = sin(angle*M_PI/180.0);
  ValueType xcorner[4];
  ValueType ycorner[4];
  xcorner[0] = (0 - xc) * ca - (0 - yc) * sa;
  ycorner[0] = (0 - xc) * sa + (0 - yc) * ca;
  xcorner[1] = (wt - xc) * ca - (0 - yc) * sa;
  ycorner[1] = (wt - xc) * sa + (0 - yc) * ca;
  xcorner[2] = (wt - xc) * ca - (ht - yc) * sa;
  ycorner[2] = (wt - xc) * sa + (ht - yc) * ca;
  xcorner[3] = (0 - xc) * ca - (ht - yc) * sa;
  ycorner[3] = (0 - xc) * sa + (ht - yc) * ca;
  ValueType xmin = std::numeric_limits<ValueType>::max();
  ValueType xmax = -std::numeric_limits<ValueType>::max();
  ValueType ymin = std::numeric_limits<ValueType>::max();
  ValueType ymax = -std::numeric_limits<ValueType>::max();
  for (uint32_t i=0;i<4;i++)
  {
    if (xcorner[i] < xmin) xmin = xcorner[i];
    if (xcorner[i] > xmax) xmax = xcorner[i];
    if (ycorner[i] < ymin) ymin = ycorner[i];
    if (ycorner[i] > ymax) ymax = ycorner[i];
  }
  uint32_t wnew = static_cast<uint32_t>(xmax - xmin) + 1;
  uint32_t hnew = static_cast<uint32_t>(ymax - ymin) + 1;
  auto img = std::make_shared<FitsImage>(image->getName(),wnew,hnew,image->getDepth());
  img->setMetadata(image->getMetadata());
  ConstPixelIterator it = image->getConstPixelIterator();
  for (int32_t y=0;y<static_cast<int32_t>(image->getHeight());y++)
  {
    for (int32_t x=0;x<static_cast<int32_t>(image->getWidth());x++)
    {
      ValueType xr = (x - xc) * ca - (y - yc) * sa - xmin;
      ValueType yr = (x - xc) * sa + (y - yc) * ca - ymin;
      uint32_t xi = static_cast<uint32_t>(xr);
      uint32_t yi = static_cast<uint32_t>(yr);
      if (xr >= 0 && xi < img->getWidth() && yr >= 0 && yi < img->getHeight())
      {
        ValueType fx = xr - xi;
        ValueType fy = yr - yi;
        for (uint32_t d=0;d<image->getDepth();d++)
        {
          ValueType v = it[d];
          img->getPixelIterator(xi,yi)[d] += v * (1 - fx) * (1 - fy);
          img->getPixelIterator(xi,yi+1)[d] += v * (1 - fx) * fy;
          img->getPixelIterator(xi+1,yi)[d] += v * fx * (1 - fy);
          img->getPixelIterator(xi+1,yi+1)[d] += v * fx * fy;
        }
      }
      ++it;
    }
  }
  if (crop)
  {
    int x = (wnew - wt) / 2;
    int y = (hnew - ht) / 2;
    img = img->subImage(QRect(x,y,wt,ht));
  }
  *image = *img;
}
