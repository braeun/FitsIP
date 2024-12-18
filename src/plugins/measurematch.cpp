/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to match two images                                          *
 *                                                                              *
 * modified: 2022-12-03                                                         *
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

#include "measurematch.h"
#include "measurematchdialog.h"
#include "opgrow.h"
#include <fitsbase/imagecollection.h>
#include <fitsbase/fileobject.h>
#include <fitsbase/fitsimage.h>
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <QDebug>

MeasureMatch::MeasureMatch():
  matchFull(true),
  matchRange(100),
  subsample(1),
  firstPassDelta(1),
  factor(1),
  aoiShiftX(0),
  aoiShiftY(0),
  dlg(nullptr)
{
  profiler = SimpleProfiler("MeasureMatch");
}

MeasureMatch::~MeasureMatch()
{
}

QString MeasureMatch::getMenuEntry() const
{
  return "Measure/Match...";
}

OpPlugin::ResultType MeasureMatch::execute(std::shared_ptr<FitsImage> image, QRect aoi, const PreviewOptions& opt)
{
  if (dlg == nullptr)
  {
    dlg = new MeasureMatchDialog();
    dlg->setImageCollection(&ImageCollection::getGlobal());
  }
  if (dlg->exec())
  {
    matchFull = dlg->isMatchFullImage();
    matchRange = dlg->getMatchRange();
    firstPassDelta = dlg->getFirstPassDelta();
    subsample = dlg->getSubsample();
    factor = dlg->getScaleFactor();
    profiler.start();
    setTemplate(image,aoi);
    std::shared_ptr<FileObject> file = dlg->getImage();
    computeMatch(file->getImage());
    profiler.stop();
    log(image,QString::asprintf("Maximum match with %s: %f at [%.1f,%.1f] shifted by [%.1f,%.1f]",file->getImage()->getName().toStdString().c_str(),max,getX(),getY(),getDx(),getDy()));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void MeasureMatch::setTemplate(std::shared_ptr<FitsImage> image, QRect a)
{
  aoi = a;
  if (aoi.isEmpty()) /* if no AOI specified, use the center of the image as the template */
  {
    qWarning() << "No AOI set - using center of active image for template";
    uint32_t w = 100;
    if (w >= image->getWidth()/2) w = image->getWidth()/2;
    uint32_t h = 100;
    if (h >= image->getHeight()/2) h = image->getHeight()/2;
    aoi = QRect((image->getWidth()-w)/2,(image->getHeight()-h)/2,w,h);
  }
  double sumR = 0;
  double sumR2 = 0;
  QRect r(0,0,image->getWidth(),image->getHeight());
  if (!r.contains(aoi))
  {
    qWarning() << "AOI not fully contained in image! Creating intersection.";
    aoi = r.intersected(aoi);
  }
  std::shared_ptr<FitsImage> img;
  if (factor > 1)
  {
    OpGrow op;
    img = op.grow(image,factor,true);
    aoi = QRect(aoi.x()*factor,aoi.y()*factor,aoi.width()*factor,aoi.height()*factor);
  }
  else
  {
    img = image;
  }
  R.clear();
  for (int i=0;i<aoi.height();i++)
  {
    R.push_back(std::vector<ValueType>(aoi.width(),0.0));
  }
  for (int32_t i=0;i<aoi.width();i++)
  {
    for (int32_t j=0;j<aoi.height();j++)
    {
      {
        double v = img->getConstPixelIterator(i+aoi.x(),j+aoi.y()).getAbs();
        R[j][i] = v;
        sumR += v;
        sumR2 += v * v;
      }
    }
  }
  meanR = sumR / (aoi.width() * aoi.height());
  sigmaR = sqrt(sumR2-sumR*meanR);
  initialAOI = aoi;
  aoiShiftX = 0;
  aoiShiftY = 0;
}

/*
 * Computes the match between the image I and the template image R
 */
void MeasureMatch::computeMatch(std::shared_ptr<FitsImage> image)
{
  createI(image);
  int xMax = width - aoi.width();
  int yMax = height - aoi.height();
  int roiXC = aoi.width() / 2;
  int roiYC = aoi.height() / 2;
  max = -std::numeric_limits<double>::max();
  x = -1;
  y = -1;
  int delta = firstPassDelta;
  for (int r=0;r<xMax;r+=delta)
  {
    for (int s=0;s<yMax;s+=delta)
    {
      double q = getMatchValue(r,s,subsample);
      if (q > max)
      {
        max = q;
        x = r + roiXC + offsetX;
        y = s + roiYC + offsetY;
      }
    }
  }
  if (delta > 1)
  {
    for (int r=std::max(x-roiXC-offsetX-2*delta,0);r<std::min(x-roiXC-offsetX+2*delta,xMax);r++)
    {
      for (int s=std::max(y-roiYC-offsetY-2*delta,0);s<std::min(y-roiYC-offsetY+2*delta,yMax);s++)
      {
        double q = getMatchValue(r,s,1);
        if (q > max)
        {
          max = q;
          x = r + roiXC + offsetX;
          y = s + roiYC + offsetY;
        }
      }
    }
  }
  dx = x - aoi.x() - roiXC;
  dy = y - aoi.y() - roiYC;
}

void MeasureMatch::setMatchFull(bool flag)
{
  matchFull = flag;
}

bool MeasureMatch::isMatchFull() const
{
  return matchFull;
}

void MeasureMatch::setMatchRange(int32_t r)
{
  matchRange = r;
}

int32_t MeasureMatch::getMatchRange() const
{
  return matchRange;
}

void MeasureMatch::setFirstPassDelta(int32_t d)
{
  firstPassDelta = d;
}

int32_t MeasureMatch::getFirstPassDelta() const
{
  return firstPassDelta;
}

void MeasureMatch::setSubsample(int32_t s)
{
  subsample = s;
}

int32_t MeasureMatch::getSubsample() const
{
  return subsample;
}

void MeasureMatch::setFactor(int32_t f)
{
  factor = f;
}

int32_t MeasureMatch::getFactor() const
{
  return factor;
}

double MeasureMatch::getX() const
{
  return x / static_cast<double>(factor);
}

double MeasureMatch::getY() const
{
  return y / static_cast<double>(factor);
}

double MeasureMatch::getDx() const
{
  return (dx + aoiShiftX) / static_cast<double>(factor);
}

double MeasureMatch::getDy() const
{
  return (dy + aoiShiftY) / static_cast<double>(factor);
}

void MeasureMatch::shiftAOI(double dx, double dy)
{
  int idx = static_cast<int>(std::round(dx*factor));
  int idy = static_cast<int>(std::round(dy*factor));
  aoi = initialAOI.translated(idx,idy);
  aoiShiftX = idx;
  aoiShiftY = idy;
}




void MeasureMatch::createI(std::shared_ptr<FitsImage> image)
{
  std::shared_ptr<FitsImage> img;
  if (factor > 1)
  {
    OpGrow op;
    img = op.grow(image,factor,true);
  }
  else
  {
    img = image;
  }
  offsetX = 0;
  offsetY = 0;
  height = img->getHeight();
  width = img->getWidth();
  if (!matchFull)
  {
    offsetX = std::max(aoi.x()-matchRange,0);
    offsetY = std::max(aoi.y()-matchRange,0);
    height = std::min(aoi.height()+2*matchRange,static_cast<int32_t>(img->getHeight())-offsetY);
    width = std::min(aoi.width()+2*matchRange,static_cast<int32_t>(img->getWidth())-offsetX);
  }
  I.clear();// = hmath::Matrix(height,width);
  for (int iy=0;iy<height;iy++)
  {
    if (offsetY+iy < 0) continue;
    if (offsetY+iy >= img->getHeight()) break;
    I.push_back(std::vector<ValueType>(width,0.0));
    for (int ix=0;ix<width;ix++)
    {
      if (offsetX+ix < 0) continue;
      if (offsetX+ix >= img->getWidth()) break;
      I[iy][ix] = img->getConstPixelIterator(offsetX+ix,offsetY+iy).getAbs();
    }
  }
}

/**
 * Calculates the match value between the template image R and the same
 * region in the image I oroginating at the given pixel coordinates
 * @param r the x coordinate in the image I
 * @param s the y coordinate in the image I
 * @param n take only every n-th pixel for matching
 * @return the correlation value
 */
double MeasureMatch::getMatchValue(int r, int s, int n)
{
  double sumI = 0;
  double sumI2= 0;
  double covIR = 0;
  for (int ix=0;ix<aoi.width();ix+=n)
  {
    for (int iy=0;iy<aoi.height();iy+=n)
    {
      double vR = R[iy][ix];
      double vI = I[s+iy][r+ix];
      sumI += vI;
      sumI2 += vI * vI;
      covIR += vI * vR;
    }
  }
  double meanI = sumI / (aoi.width() * aoi.height());
  double sigmaI = (double)sqrt(sumI2 - sumI * meanI);
  if (abs(sigmaI) < 1E-10) return 0;
  return (covIR - sumI * meanR) / (sigmaI * sigmaR);
}



