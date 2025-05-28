/********************************************************************************
 *                                                                              *
 * FitsIP - stack images                                                        *
 *                                                                              *
 * modified: 2025-03-15                                                         *
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

#include "opstack.h"
#include "opstackdialog.h"
#include <fitsip/coreplugins/oprotate.h>
#include <fitsip/coreplugins/opshift.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/io/iofactory.h>
#include <algorithm>
#include <QApplication>
#include <QDebug>

OpStack::OpStack():
  dlg(nullptr),
  subtractSky(true),
  searchbox(100),
  starbox(20),
  maxmove(20),
  rotate(false)
{
  profiler = SimpleProfiler("OpStack");
}

OpStack::~OpStack()
{
  if (dlg) delete dlg;
}

bool OpStack::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsObject>> OpStack::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}

QString OpStack::getMenuEntry() const
{
  return "Math/Stack...";
}

bool OpStack::requiresFileList() const
{
  return true;
}

OpPlugin::ResultType OpStack::execute(const std::vector<QFileInfo>& list, const OpPluginData& data)
{
  if (list.size() < 2)
  {
    qWarning() << "Not enough files in list for stacking";
    return ERROR;
  }
  if (!dlg) dlg = new OpStackDialog();
  if (dlg->exec())
  {
    Align mode = static_cast<Align>(dlg->getAlignment());
    QString msg = "";
    switch (mode)
    {
      case Align::NoAlignment:
      default:
        msg = "no alignment";
        break;
      case Align::TemplateMatch:
        msg = "template matching";
        break;
      case Align::StarMatch:
        msg = "star matching";
        break;
    }

    ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
    if (prog)
    {
      prog->setMaximum(list.size());
      prog->setProgress(0);
      prog->appendMessage(list[0].fileName());
      prog->show();
      QApplication::processEvents();
    }
    else
      QApplication::setOverrideCursor(Qt::BusyCursor);
    profiler.start();
    ResultType ret;
    switch (mode)
    {
      case Align::NoAlignment:
      default:
        ret = prepare(list[0],dlg->isSubtractSky());
        break;
      case Align::TemplateMatch:
        ret = prepareTemplate(list[0],dlg->isSubtractSky(),data.aoi,dlg->isFullTemplateMatch(),dlg->getTemplateMatchRange());
        break;
      case Align::StarMatch:
        ret = prepareStarMatch(list[0],data.pixellist,dlg->isSubtractSky(),dlg->getSearchBoxSize(),dlg->getStarBoxSize(),dlg->isAllowRotation(),dlg->getStarMaxMovement());
        break;
    }
    if (ret != OK)
    {
      if (prog) prog->deleteLater();
      QApplication::restoreOverrideCursor();
      return ret;
    }
    log(img,list[0].baseName()+" loaded as base for stacking");
    for (size_t i=1;i<list.size();++i)
    {
      const QFileInfo& file = list[i];
      switch (mode)
      {
        case Align::NoAlignment:
        default:
          ret = stack(file);
          break;
        case Align::TemplateMatch:
          ret = stackTemplate(file);
          break;
        case Align::StarMatch:
          ret = stackStarMatch(file);
          break;
      }
      if (prog)
      {
        prog->setProgress(i);
        prog->appendMessage(file.fileName()+(ret==OK?" - Success":" - Error"));
        QApplication::processEvents();
        if (prog->isCancelled()) break;
      }
    }
    profiler.stop();
    if (img) logProfiler(img,msg);
    if (prog) prog->deleteLater();
    QApplication::restoreOverrideCursor();
    return OK;
  }
  return CANCELLED;
}

OpPlugin::ResultType OpStack::prepare(const QFileInfo& file, bool subsky)
{
  subtractSky = subsky;
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qCritical() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  try
  {
    img = std::make_shared<FitsImage>("stack",*handler->read(file.absoluteFilePath()));
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img.get());
      AverageResult avg = hist.getAverage(0.75);
      *img -= avg.mean;
    }
  }
  catch (std::exception& ex)
  {
    qCritical() << ex.what();
    return ERROR;
  }
  return OK;
}

OpPlugin::ResultType OpStack::prepareTemplate(const QFileInfo &file, bool subsky, QRect aoi, bool full, int range)
{
  OpPlugin::ResultType res = prepare(file,subsky);
  if (res == OK)
  {
    matcher.setMatchFull(full);
    matcher.setMatchRange(range);
    matcher.setTemplate(img,aoi);
  }
  return res;
}

OpPlugin::ResultType OpStack::prepareStarMatch(const QFileInfo &file, PixelList* pixellist, bool subsky, int searchbox, int starbox, bool rotate, double maxmove)
{
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
  OpPlugin::ResultType res = prepare(file,subsky);
  if (res == OK)
  {
    std::vector<Star> stars;
    for (const Pixel& pixel : pixellist->getPixels())
    {
      Star star(pixel.x,pixel.y);
      stars.push_back(star);
    }
    starlist1.setStars(stars);
    findStars(img,&starlist1,searchbox,starbox);
    starlist2.setStars(starlist1.getStars());
  }
  return res;
}

OpPlugin::ResultType OpStack::stack(const QFileInfo &file)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qWarning() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  try
  {
    auto img1 = handler->read(file.absoluteFilePath());
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img1.get());
      AverageResult avg = hist.getAverage(0.75);
      *img1 -= avg.mean;
    }
    *img += *img1;
    log(img,"stacked  "+img1->getName());
    qInfo() << "Stacked: " << img1->getName();
  }
  catch (std::exception& ex)
  {
    qWarning() << ex.what();
    return ERROR;
  }
  return OK;
}

OpPlugin::ResultType OpStack::stackTemplate(const QFileInfo &file)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qWarning() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  try
  {
    auto img1 = handler->read(file.absoluteFilePath());
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img1.get());
      AverageResult avg = hist.getAverage(0.75);
      *img1 -= avg.mean;
    }
    matcher.computeMatch(img1);
    OpShift shift;
    shift.shift(img1,-matcher.getDx(),-matcher.getDy());
    *img += *img1;
    log(img,QString::asprintf("stacked %s shifted by [%.1f,%.1f]",img1->getName().toUtf8().data(),-matcher.getDx(),-matcher.getDy()));
    qInfo() << "Stacked: " << img1->getName();
  }
  catch (std::exception& ex)
  {
    qWarning() << ex.what();
    return ERROR;
  }
  return OK;
}

OpPlugin::ResultType OpStack::stackStarMatch(const QFileInfo &file)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qWarning() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  try
  {
    auto img1 = handler->read(file.absoluteFilePath());
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img1.get());
      AverageResult avg = hist.getAverage(0.75);
      *img1 -= avg.mean;
    }
    /* save starlist of last image */
    StarList last;
    last.setStars(starlist2.getStars());
    /* get stars for current image */
    findStars(img1,&starlist2,searchbox,starbox);
    StarList tmp;
    tmp.setStars(starlist2.getStars());
    double angle = 0;
    double angleSigma = 0;
    if (rotate && starlist1.getStars().size() > 1 && starlist2.getStars().size())
    {
      auto a = getRotationAngle(starlist1,starlist2);
      std::tie(angle,angleSigma) = a;
      if (fabs(angle) > 0.001)
      {
        OpRotate rot;
        rot.rotate(img1,angle,true);
        int32_t xc = static_cast<int32_t>(img1->getWidth()) / 2;
        int32_t yc = static_cast<int32_t>(img1->getHeight()) / 2;
        tmp.rotate(xc,yc,angle);
      }
    }
    double dx,sigmadx,dy,sigmady;
    /* get shift to last image to check againt maxmove */
    std::tie(dx,sigmadx,dy,sigmady) = getShift(last,starlist2);
    if (fabs(dx) > maxmove || fabs(dy) > maxmove)
    {
      qWarning() << img->getName() << " shift " << dx << dy << "exceeds max move value" << maxmove;
      return ERROR;
    }
    /* using the new rotated starlist, get shift to first image for stacking */
    std::tie(dx,sigmadx,dy,sigmady) = getShift(starlist1,tmp);
    OpShift shift;
    shift.shift(img1,-dx,-dy);
    *img += *img1;
    if (rotate)
      log(img,QString::asprintf("stacked %s rotated by %.3f°+-%.3f° shifted by [%.1f+-%.2f,%.1f+-%.2f]",img1->getName().toUtf8().data(),angle,angleSigma,-dx,sigmadx,-dy,sigmady));
    else
      log(img,QString::asprintf("stacked %s shifted by [%.1f+-%.2f,%.1f+-%.2f]",img1->getName().toUtf8().data(),-dx,sigmadx,-dy,sigmady));
    qInfo() << "Stacked: " << img1->getName();
  }
  catch (std::exception& ex)
  {
    qWarning() << ex.what();
    return ERROR;
  }
  return OK;
}

void OpStack::findStars(std::shared_ptr<FitsImage> image, StarList *starlist, int searchbox, int starbox)
{
  double sky = 0;
  if (!subtractSky)
  {
    /* The sky background has not been subtracted yet.
     * Set the mean value for the findStars function. */
    Histogram hist;
    hist.build(image.get());
    AverageResult avg = hist.getAverage(0.75);
    sky = avg.mean;
  }
  std::vector<Star> list;
  for (const Star& star : starlist->getStars())
  {
    Star s = findStar(image,sky,star.getX(),star.getY(),searchbox,starbox);
    list.push_back(s);
  }
  starlist->setStars(list);
}

Star OpStack::findStar(std::shared_ptr<FitsImage> image, double sky, int x, int y, int searchbox, int starbox)
{
  QRect r(x-searchbox/2,y-searchbox/2,searchbox,searchbox);
  r = image->getOverlap(r);
  /* find the brightest pixel in the rectangle, assuming thats the new position of the star */
  Pixel pixel = image->getBrightestPixel(r);
  QRect sr(pixel.x-starbox/2,pixel.y-starbox/2,starbox,starbox);
  sr = image->getOverlap(sr);
  int maxiter = 20;
  double xc;
  double yc;
  double fwhm;
  double xwidth;
  double ywidth;
  starfinder.starAxes(image,sr,sky,&xc,&yc,&fwhm,&xwidth,&ywidth,maxiter);
  Star star(xc,yc,fwhm,xwidth,ywidth,0,0,0);
//  qDebug() << "findStar " << image->getName() << sky << x << y << r << xc << yc << xwidth << ywidth;
  return star;
}

/*
 * calculate the rotation angle in degrees.
 * Returns a tuple with <angle,stddev>
 */
std::tuple<double,double> OpStack::getRotationAngle(const StarList &list1, const StarList &list2)
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
  qDebug() << QString::asprintf("StackOp: rotation angle = %f +- %f\n",angle,stddev);
  return std::tuple(angle,stddev);
}

/*
 * Get the amount by wich list2 is shifted with respect to list1.
 */
std::tuple<double,double,double,double> OpStack::getShift(const StarList& list1, const StarList& list2)
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
