/********************************************************************************
 *                                                                              *
 * FitsIP - shift image with subpixel accuracy                                  *
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

#include "opshift.h"
#include "opshiftdialog.h"
#include <fitsbase/fitsimage.h>
#include <algorithm>

OpShift::OpShift():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpShift");
}

OpShift::~OpShift()
{
  if (dlg) dlg->deleteLater();
}

QString OpShift::getMenuEntry() const
{
  return "Image/Shift...";
}

QIcon OpShift::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/transform-move.png");
}

OpPlugin::ResultType OpShift::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/, const PreviewOptions& opt)
{
  if (dlg == nullptr) dlg = new OpShiftDialog();
  dlg->setImageSize(image->getWidth(),image->getHeight());
  if (dlg->exec())
  {
    ValueType dx = dlg->getDeltaX();
    ValueType dy = dlg->getDeltaY();
    profiler.start();
    shift(image,dx,dy);
    profiler.stop();
    log(image,QString("OpShift: dx=%1  dy=%2").arg(dx).arg(dy));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpShift::shift(std::shared_ptr<FitsImage> image, ValueType dx, ValueType dy)
{
  FitsImage img(*image);
  PixelIterator it2 = image->getPixelIterator();
  int32_t yi = static_cast<int32_t>(-dy);
  ValueType yf = -dy - yi;
  int32_t xi = static_cast<int32_t>(-dx);
  ValueType xf = -dx - xi;
  for (uint32_t y0=0;y0<image->getHeight();y0++)
  {
    if (yi < 0 || yi+1 >= image->getHeight())
    {
      for (uint32_t x0=0;x0<image->getWidth();x0++)
      {
        it2.clear();
        ++it2;
      }
    }
    else
    {
      xi = static_cast<int32_t>(-dx);
      ConstPixelIterator it1 = img.getConstPixelIterator(std::max(0,xi),yi);
      for (uint32_t x0=0;x0<image->getWidth();x0++)
      {
        if (xi < 0 || xi+1 >= image->getWidth())
        {
          it2.clear();
        }
        else
        {
          for (uint32_t l=0;l<image->getDepth();l++)
          {
            ValueType a = it1[l]; //pixel[yi][xi];
            ValueType b = (it1+image->getWidth())[l]; // pixel[yi+1][xi];
            ValueType c = (it1+1)[l]; // pixel[yi][xi+1];
            ValueType d = (it1+image->getWidth()+1)[l]; // pixel[yi+1][xi+1];
            it2[l] = a * (1 - xf) * (1 - yf) + b * (1 - xf) * yf + c * xf * (1 - yf) + d * xf * yf;
          }
          ++it1;
        }
        ++xi;
        ++it2;
      }
    }
    ++yi;
  }
}

