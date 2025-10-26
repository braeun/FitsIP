/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to match two images based on stars                           *
 *                                                                              *
 * modified: 2025-10-26                                                         *
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

#include "starmatcher.h"
#include "starmatcherdialog.h"
#include <fitsip/core/imagecollection.h>
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/fitsimage.h>
#include <cmath>
#include <algorithm>
#include <QDebug>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

StarMatcher::StarMatcher():
  dlg(nullptr),
  subsky(true),
  searchbox(100),
  starbox(20),
  maxmove(20),
  rotate(false)
{
}

StarMatcher::~StarMatcher()
{
}

QString StarMatcher::getMenuEntry() const
{
  return "Analyse/Match Stars...";
}

OpPlugin::ResultType StarMatcher::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (dlg == nullptr)
  {
    dlg = new StarMatcherDialog();
    dlg->setImageCollection(data.imageCollection);
  }
  if (dlg->exec())
  {
    profiler.start();
    std::shared_ptr<FitsObject> file = dlg->getImage();
    auto ret = prepare(image->getImage(),data.pixellist,dlg->isSubtractSky(),dlg->getSearchBoxSize(),dlg->getStarBoxSize(),dlg->isAllowRotation(),dlg->getStarMaxMovement());
    if (ret == OK)
    {
      ret = match(file->getImage());
    }
    profiler.stop();
    if (ret == OK)
    {
      log(image,QString::asprintf("Match with %s: shifted by [%.1f+-%.1f , %.1f+-%.1f], rotation=%.1f+-%.1f°",
                                   file->getImage()->getName().toStdString().c_str(),getDx(),getSigmadx(),getDy(),getSigmady(),
                                  getAngle()*180/M_PI,getAngleSigma()*180/M_PI));
    }
    logProfiler(image);
    return ret;
  }
  return CANCELLED;
}

OpPlugin::ResultType StarMatcher::prepare(std::shared_ptr<FitsImage> image, PixelList* pixellist, bool subsky, int searchbox, int starbox, bool rotate, double maxmove)
{
  this->subsky = subsky;
  if (pixellist->getPixels().empty())
  {
    qWarning() << "No stars selected for star matching";
    return ERROR;
  }
  if (searchbox <= 0)
  {
    qWarning() << "searchbox size must be >0";
    return ERROR;
  }
  this->searchbox = searchbox;
  if (starbox <= 0)
  {
    qWarning() << "starbox size must be >0";
    return ERROR;
  }
  this->starbox = starbox;
  this->rotate = rotate;
  this->maxmove = maxmove;
  auto img = image->toGray();
  if (subsky)
  {
    Histogram hist;
    hist.build(img.get());
    AverageResult avg = hist.getAverage(0.75);
    *img -= avg.mean;
  }
  std::vector<Star> stars = starfinder.findStars(img,pixellist,0,starbox);
  starlist1.setStars(stars);
  starlist2.setStars(starlist1.getStars());
  return OK;
}

OpPlugin::ResultType StarMatcher::match(std::shared_ptr<FitsImage> image)
{
  angle = 0;
  angleSigma = 0;
  dx = 0;
  sigmadx = 0;
  dy = 0;
  sigmady = 0;
  /* save starlist of last image */
  StarList last;
  last.setStars(starlist2.getStars());
  auto img = image->toGray();
  if (!subsky)
  {
    /* The sky background has not been subtracted yet.
     * Set the mean value for the findStars function. */
    Histogram hist;
    hist.build(image.get());
    AverageResult avg = hist.getAverage(0.75);
    *img -= avg.mean;
  }
  PixelList pixellist;
  for (const Star& star : starlist2.getStars())
  {
    QRect r(star.getX()-searchbox/2,star.getY()-searchbox/2,searchbox,searchbox);
    r = image->getOverlap(r);
    /* find the brightest pixel in the rectangle, assuming thats the new position of the star */
    Pixel pixel = image->getBrightestPixel(r);
    pixellist.addPixel(pixel);
  }
  std::vector<Star> stars = starfinder.findStars(img,&pixellist,0,starbox);
  starlist2.setStars(stars);
  StarList tmp;
  tmp.setStars(starlist2.getStars());
  if (rotate && starlist1.getStars().size() > 1 && starlist2.getStars().size() > 1)
  {
    auto a = getRotationAngle(starlist1,starlist2);
    std::tie(angle,angleSigma) = a;
    if (fabs(angle) > 0.001)
    {
      int xc = static_cast<int>(img->getWidth()) / 2;
      int yc = static_cast<int>(img->getHeight()) / 2;
      tmp.rotate(xc,yc,angle);
    }
  }
  /* get shift to last image to check againt maxmove */
  std::tie(dx,sigmadx,dy,sigmady) = getShift(last,starlist2);
  if (fabs(dx) > maxmove || fabs(dy) > maxmove)
  {
    qWarning() << img->getName() << " shift " << dx << dy << "exceeds max move value" << maxmove;
    return ERROR;
  }
  /* using the new rotated starlist, get shift to first image for stacking */
  std::tie(dx,sigmadx,dy,sigmady) = getShift(starlist1,tmp);
  return OK;
}

double StarMatcher::getAngle() const
{
  return angle;
}

double StarMatcher::getAngleSigma() const
{
  return angleSigma;
}

double StarMatcher::getDx() const
{
  return dx;
}

double StarMatcher::getSigmadx() const
{
  return sigmadx;
}

double StarMatcher::getDy() const
{
  return dy;
}

double StarMatcher::getSigmady() const
{
  return sigmady;
}







/*
 * calculate the rotation angle in degrees.
 * Returns a tuple with <angle,stddev>
 */
std::tuple<double,double> StarMatcher::getRotationAngle(const StarList &list1, const StarList &list2)
{
  double sum = 0.0;
  double sum2 = 0.0;
  const std::vector<Star> s1 = list1.getStars();
  const std::vector<Star> s2 = list2.getStars();
  int n = std::min(s1.size(),s2.size()) - 1;
  for (int i=0;i<n;i++)
  {
    double a1 = atan2(s1[i+1].getY()-s1[i].getY(),s1[i+1].getX()-s1[i].getX());
    if (a1 < 0) a1 += 2.0 * M_PI;
    double a2 = atan2(s2[i+1].getY()-s2[i].getY(),s2[i+1].getX()-s2[i].getX());
    if (a2 < 0) a2 += 2.0 * M_PI;
    double a = a1 - a2;
    sum += a;
    sum2 += a * a;
  }
  double angle = sum / n * 180.0 / M_PI;
  double stddev = 0.0;
  if (n > 1) stddev = sqrt((n*sum2-sum*sum)/n/(n-1));
  stddev *= 180.0 / M_PI;
//  qDebug() << QString::asprintf("StarMatcher: rotation angle = %f +- %f\n",angle,stddev);
  return std::tuple(angle,stddev);
}

/*
 * Get the amount by wich list2 is shifted with respect to list1.
 */
std::tuple<double,double,double,double> StarMatcher::getShift(const StarList& list1, const StarList& list2)
{
  const std::vector<Star> s1 = list1.getStars();
  const std::vector<Star> s2 = list2.getStars();
  int64_t n = std::min(s1.size(),s2.size());
  double dx = 0;
  double dx2 = 0;
  double dy = 0;
  double dy2 = 0;
  for (int i=0;i<n;i++)
  {
    double d = s2[i].getX() - s1[i].getX();
    dx += d;
    dx2 += d * d;
    d = s2[i].getY() - s1[i].getY();
    dy += d;
    dy2 += d * d;
  }
  double stddevDx = 0.0;
  double stddevDy = 0.0;
  if (n > 1)
  {
    stddevDx = sqrt((n*dx2-dx*dx)/n/(n-1));
    stddevDy = sqrt((n*dy2-dy*dy)/n/(n-1));
  }
  dx /= n;
  dy /= n;
  return std::tuple(dx,stddevDx,dy,stddevDy);
}

