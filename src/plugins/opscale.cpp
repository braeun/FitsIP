/********************************************************************************
 *                                                                              *
 * FitsIP - linear scaling of image intensity                                   *
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

#include "opscale.h"
#include <fitsimage.h>

OpScale::OpScale()
{
  dlg.setTitle("Scale Image");
  dlg.setValue1Label("Bias:");
  dlg.setValue2Label("Scale:");
  dlg.setValue1("0.0");
  dlg.setValue2("1.0");
  profiler = SimpleProfiler("OpScale");
}

OpScale::~OpScale()
{
}

QString OpScale::getMenuEntry() const
{
  return "Math/Scale...";
}

OpPlugin::ResultType OpScale::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  if (dlg.exec())
  {
    ValueType bias = dlg.getValue1().toDouble();
    ValueType scale = dlg.getValue2().toDouble();
    profiler.start();
    uint32_t n = image->getWidth() * image->getHeight();
    for (uint32_t i=0;i<image->getDepth();i++)
    {
      ValueType* p = image->getLayer(i)->getData();
      for (uint32_t j=0;j<n;j++)
      {
        *p = *p * scale + bias;
        p++;
      }
    }
    profiler.stop();
    log(image,QString("scaled image: scale=%1 bias=%2").arg(scale).arg(bias));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}
