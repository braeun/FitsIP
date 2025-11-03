/********************************************************************************
 *                                                                              *
 * FitsIP - create an average of several images                                 *
 *                                                                              *
 * modified: 2025-10-24                                                         *
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

#include "opaverage.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/io/iofactory.h>
#include <QApplication>
#include <QDebug>

OpAverage::OpAverage()
{
  profiler = SimpleProfiler("OpAverage");
}

OpAverage::~OpAverage()
{
}

std::vector<std::shared_ptr<FitsObject>> OpAverage::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}

QString OpAverage::getMenuEntry() const
{
  return "Math/Average";
}

bool OpAverage::requiresFileList() const
{
  return true;
}

OpPlugin::ResultType OpAverage::execute(const std::vector<QFileInfo>& list, const OpPluginData& data)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
    QApplication::processEvents();
  }
  else
    QApplication::setOverrideCursor(Qt::BusyCursor);
  profiler.start();
  int n = 0;
  for (size_t i=0;i<list.size();i++)
  {
    if (add(list[i]) == OK) n++;
    if (prog)
    {
      prog->setProgress(i);
      prog->appendMessage(list[i].fileName());
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
  }
  if (n == 0) return ERROR;
  *img /= n;
  profiler.stop();
  log(img,QString("Divided image by %1").arg(n));
  logProfiler(img);
  if (prog) prog->deleteLater();
  QApplication::restoreOverrideCursor();
  return OK;
}

OpPlugin::ResultType OpAverage::add(const QFileInfo& file)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.absoluteFilePath());
  if (!handler)
  {
    qWarning() << "No handler found for"  << file.absoluteFilePath();
    return ERROR;
  }
  try
  {
    if (!img)
    {
      img = std::make_shared<FitsImage>("average",*handler->read(file.absoluteFilePath()).front()->getImage());
    }
    else
    {
      *img += *handler->read(file.absoluteFilePath()).front()->getImage();
    }
    log(img,"Added image "+file.fileName());
  }
  catch (std::exception& ex)
  {
    qWarning() << ex.what();
    return ERROR;
  }
  return OK;
}

