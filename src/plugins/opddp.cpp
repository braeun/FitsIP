/********************************************************************************
 *                                                                              *
 * FitsIP - digital development processing                                      *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#include "opddp.h"
#include "opddpdialog.h"
#include "opgaussblur.h"
#include <fitsbase/fitsimage.h>
#include <cmath>

OpDDP::OpDDP():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpDDP");
}

OpDDP::~OpDDP()
{
}

QString OpDDP::getMenuEntry() const
{
  return "Filter/DDP...";
}

OpPlugin::ResultType OpDDP::execute(std::shared_ptr<FitsImage> image, QRect selection)
{
  if (!dlg) dlg = new OpDDPDialog();
  dlg->setPreview(image,selection);
  if (dlg->exec())
  {
    ValueType sigma = dlg->getSigma();
    ValueType bkg = dlg->getBackground();
    ValueType a = dlg->getA();
    ValueType b = dlg->getB();
    profiler.start();
    ddp(image,sigma,bkg,a,b);
//    auto i1 = std::make_shared<FitsImage>(*image);
//    *i1 -= bkg;
//    OpGaussBlur blur;
//    blur.blur(i1,sigma,sigma);
//    *i1 += a;
//    *image += b;
//    *image /= *i1;
    profiler.stop();
    log(image,QString::asprintf("DDP: sigma=%f  bkg=%f  a=%f  b=%f",sigma,bkg,a,b));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpDDP::ddp(std::shared_ptr<FitsImage> image, ValueType sigma, ValueType bkg, ValueType a, ValueType b)
{
  auto i1 = std::make_shared<FitsImage>(*image);
  *i1 -= bkg;
  OpGaussBlur blur;
  blur.blur(i1,sigma,sigma);
  *i1 += a;
  *image += b;
  *image /= *i1;
}
