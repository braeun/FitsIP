/********************************************************************************
 *                                                                              *
 * FitsIP - stack images                                                        *
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

#include "opstack.h"
#include "opstackdialog.h"
#include <fitsip/coreplugins/oprotate.h>
#include <fitsip/coreplugins/opshift.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/io/iofactory.h>
#include <QApplication>
#include <QDebug>

OpStack::OpStack():
  dlg(nullptr),
  subtractSky(true),
  // searchbox(100),
  // starbox(20),
  // maxmove(20),
  rotate(false)
{
  profiler = SimpleProfiler("OpStack");
}

OpStack::~OpStack()
{
  if (dlg) delete dlg;
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
        rotate = dlg->isAllowRotation();
        ret = prepareStarMatch(list[0],data.pixellist,dlg->isSubtractSky(),dlg->getSearchBoxSize(),dlg->getStarBoxSize(),dlg->isAllowRotation(),dlg->getStarMaxMovement());
        break;
    }
    if (ret != OK)
    {
      if (prog) prog->deleteLater();
      QApplication::restoreOverrideCursor();
      return ret;
    }
    log(&img,list[0].baseName()+" loaded as base for stacking");
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
    img = FitsImage("stack",handler->read(file.absoluteFilePath()).front()->getImage());
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img);
      AverageResult avg = hist.getAverage(0.75);
      img -= avg.mean;
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
  OpPlugin::ResultType res = prepare(file,subsky);
  if (res == OK)
  {
    res = starmatcher.prepare(img,pixellist,false,searchbox,starbox,rotate,maxmove);
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
    auto img1 = handler->read(file.absoluteFilePath()).front()->getImage();
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img1);
      AverageResult avg = hist.getAverage(0.75);
      img1 -= avg.mean;
    }
    img += img1;
    log(&img,"stacked  "+img1.getName());
    qInfo() << "Stacked: " << img1.getName();
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
    auto img1 = handler->read(file.absoluteFilePath()).front()->getImage();
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img1);
      AverageResult avg = hist.getAverage(0.75);
      img1 -= avg.mean;
    }
    matcher.computeMatch(img1);
    OpShift shift;
    shift.shift(&img1,-matcher.getDx(),-matcher.getDy());
    img += img1;
    log(&img,QString::asprintf("stacked %s shifted by [%.1f,%.1f]",img1.getName().toUtf8().data(),-matcher.getDx(),-matcher.getDy()));
    qInfo() << "Stacked: " << img1.getName();
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
    auto img1 = handler->read(file.absoluteFilePath()).front()->getImage();
    if (subtractSky)
    {
      Histogram hist;
      hist.build(img1);
      AverageResult avg = hist.getAverage(0.75);
      img1 -= avg.mean;
    }
    ResultType res = starmatcher.match(img1);
    if (res == OK)
    {
      if (rotate)
      {
        double angle = starmatcher.getAngle();
        if (fabs(angle) > 0.001 && fabs(angle) < starmatcher.getAngleSigma()/2)
        {
          OpRotate rot;
          rot.rotate(&img1,angle,true);
        }
      }
      OpShift shift;
      shift.shift(&img1,-starmatcher.getDx(),-starmatcher.getDy());
      img += img1;
      if (rotate)
        log(&img,QString::asprintf("stacked %s rotated by %.3f°+-%.3f° shifted by [%.1f+-%.2f,%.1f+-%.2f]",img1.getName().toUtf8().data(),
                                   starmatcher.getAngle(),starmatcher.getAngleSigma(),
                                   -starmatcher.getDx(),starmatcher.getSigmadx(),-starmatcher.getDy(),starmatcher.getSigmady()));
      else
        log(&img,QString::asprintf("stacked %s shifted by [%.1f+-%.2f,%.1f+-%.2f]",img1.getName().toUtf8().data(),
                                   -starmatcher.getDx(),starmatcher.getSigmadx(),-starmatcher.getDy(),starmatcher.getSigmady()));
      qInfo() << "Stacked: " << img1.getName();
    }
    else
    {
      return res;
    }
  }
  catch (std::exception& ex)
  {
    qWarning() << ex.what();
    return ERROR;
  }
  return OK;
}

