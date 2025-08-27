/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to align images                                              *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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

#include "opalign.h"
#include "opaligndialog.h"
#include <fitsip/coreplugins/opshift.h>
#include <fitsip/core/io/iofactory.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <QApplication>
#include <QDebug>
#include <QDir>

OpAlign::OpAlign():
  dlg(nullptr),
  outputPath(""),
  matchFull(false),
  matchRange(100),
  adaptAOI(false)
{
  profiler = SimpleProfiler("OpAlign");
}

OpAlign::~OpAlign()
{
  if (dlg) delete dlg;
}

QString OpAlign::getMenuEntry() const
{
  return "Math/Align...";
}

bool OpAlign::requiresFileList() const
{
  return true;
}

OpPlugin::ResultType OpAlign::execute(const std::vector<QFileInfo>& list, const OpPluginData& data)
{
  if (list.size() < 2) return CANCELLED;
  if (!dlg) dlg = new OpAlignDialog();
  if (dlg->exec())
  {
    outputPath = dlg->getOutputPath();
    matchFull = dlg->isMatchFull();
    matchRange = dlg->getMatchRange();
    adaptAOI = dlg->isAdaptAOI();
    ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
    if (prog)
    {
      prog->setMaximum(list.size());
      prog->show();
    }
    ResultType ret = prepare(list[0],data.aoi);
    if (ret != OK)
    {
      if (prog) prog->deleteLater();
      return ret;
    }
    if (prog) prog->appendMessage(list[0].fileName());
    for (size_t i=1;i<list.size();i++)
    {
      align(list[i]);
      if (prog)
      {
        prog->setProgress(i);
        prog->appendMessage(list[i].fileName());
        QApplication::processEvents();
        if (prog->isCancelled()) break;
      }
    }
    profiler.stop();
    logProfiler(list[0].baseName());
    if (prog) prog->deleteLater();
    return OK;
  }
  return CANCELLED;
}

OpPlugin::ResultType OpAlign::prepare(const QFileInfo& file, QRect aoi)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qCritical() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  if (outputPath.isEmpty())
  {
    outputPath = file.absolutePath();
  }
  std::shared_ptr<FitsImage> img;
  try
  {
    img = handler->read(file.absoluteFilePath());
    save(img,outputPath,file,"aligned");
  }
  catch (std::exception& ex)
  {
    qCritical() << ex.what();
    return ERROR;
  }
  matcher.setMatchFull(matchFull);
  matcher.setMatchRange(matchRange);
  matcher.setTemplate(img,aoi);
  QString txt = QString::asprintf("base for alignment using AOI [%d,%d+%dx%d]",aoi.x(),aoi.y(),aoi.width(),aoi.height());
  if (matchFull)
    txt += "; match full";
  else
    txt += "; match range " + QString::number(matchRange);
  if (adaptAOI) txt += "; adapt AOI";
  log(img,txt);
  return OK;
}

OpPlugin::ResultType OpAlign::align(const QFileInfo &file)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qWarning() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  try
  {
    std::shared_ptr<FitsImage> img1 = handler->read(file.absoluteFilePath());
    matcher.computeMatch(img1);
    OpShift shift;
    shift.shift(img1,-matcher.getDx(),-matcher.getDy());
    save(img1,outputPath,file,"aligned");
    log(img1,QString::asprintf("shifted by [%.1f,%.1f]",-matcher.getDx(),-matcher.getDy()));
    if (adaptAOI) matcher.shiftAOI(matcher.getDx(),matcher.getDy());
  }
  catch (std::exception& ex)
  {
    qWarning() << ex.what();
    return ERROR;
  }
  return OK;
}

