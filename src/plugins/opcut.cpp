/********************************************************************************
 *                                                                              *
 * FitsIP - cut low and high values                                             *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#include "opcut.h"
#include <fitsimage.h>

OpCut::OpCut()
{
  dlg.setTitle("Cut Image");
  dlg.setValue1Label("Lower limit:");
  dlg.setValue2Label("Upper limit:");
  dlg.setValue1("0.0");
  dlg.setValue2(QString::number(std::numeric_limits<ValueType>::max()));
  profiler = SimpleProfiler("OpCut");
}

OpCut::~OpCut()
{
}

QString OpCut::getMenuEntry() const
{
  return "Math/Cut...";
}

OpPlugin::ResultType OpCut::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  if (dlg.exec())
  {
    ValueType lower = dlg.getValue1().toDouble();
    ValueType upper = dlg.getValue2().toDouble();
    profiler.start();
    cut(image,lower,upper);
    profiler.stop();
    log(image,QString("cut values ouside range: lower=%1 upper=%2").arg(lower).arg(upper));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpCut::cut(std::shared_ptr<FitsImage> image, ValueType lower, ValueType upper)
{
  PixelIterator p = image->getPixelIterator();
  while (p.hasNext())
  {
    if (p.min() < lower)
      p.set(lower);
    else if (p.max() > upper)
      p.set(upper);
    ++p;
  }
}
