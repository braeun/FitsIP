/********************************************************************************
 *                                                                              *
 * FitsIP - image calibration with flatfield and dark image                     *
 *                                                                              *
 * modified: 2025-11-03                                                         *
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

#include "opcalibration.h"
#include "opcalibrationdialog.h"
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/imagecollection.h>
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/io/iofactory.h>
#include <QApplication>
#include <QDebug>
#include <QDir>

OpCalibration::OpCalibration():
  dlg(nullptr)
{
}

OpCalibration::~OpCalibration()
{
}

QString OpCalibration::getMenuEntry() const
{
  return "Math/Calibrate...";
}

bool OpCalibration::requiresFileList() const
{
  return true;
}

OpPlugin::ResultType OpCalibration::execute(const std::vector<QFileInfo>& list, const OpPluginData& data)
{
  if (list.empty()) return CANCELLED;
  if (!dlg)
  {
    dlg = new OpCalibrationDialog();
  }
  dlg->setImageCollection(data.imageCollection);
  dlg->setOutputPath(list[0].absolutePath());
  if (!dlg->exec()) return CANCELLED;
  QDir dir(dlg->getOutputPath());
  if (!dir.exists())
  {
    dir.mkpath(dlg->getOutputPath());
    dir = QDir(dlg->getOutputPath());
  }
  IOHandler* handler = IOFactory::getInstance()->getHandler("tmp.fts");
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  std::shared_ptr<FitsObject> darkframe = dlg->getDarkFrame();
  std::shared_ptr<FitsObject> flatfield = dlg->getFlatField();
  double mean = 1.0;
  if (flatfield)
  {
    double w = flatfield->getImage()->getWidth() / 5;
    double h = flatfield->getImage()->getHeight() / 5;
    QRect r((flatfield->getImage()->getWidth()-w)/2,(flatfield->getImage()->getHeight()-h)/2,w,h);
    ImageStatistics stat(*flatfield->getImage(),r);
    mean = stat.getGlobalStatistics().meanValue;
  }
  int32_t n = 0;
  for (const QFileInfo& info : list)
  {
    if (prog)
    {
      prog->setProgress(n++);
      prog->appendMessage(info.fileName());
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
    try
    {
      auto img = calibrate(info,darkframe,flatfield,mean);
      if (img)
      {
        QString name = QString("%1%2%3.fts").arg(dlg->getPrefix()).arg(img->getName()).arg(dlg->getSuffix());
        handler->write(dir.filePath(name),img.get());
      }
    }
    catch (const std::exception& ex)
    {
      qWarning() << ex.what();
    }
  }
  if (prog) prog->deleteLater();
  return OK;
}



std::shared_ptr<FitsImage> OpCalibration::calibrate(const QFileInfo& info, std::shared_ptr<FitsObject> darkframe, std::shared_ptr<FitsObject> flatfield, double mean)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(info.absoluteFilePath());
  if (!handler) return std::shared_ptr<FitsImage>();
  auto img = handler->read(info.absoluteFilePath()).front()->getImageShared();
  if (darkframe)
  {
    *img -= *darkframe->getImage();
    log(img.get(),"Subtracted darkframe '"+darkframe->getImage()->getName()+"'");
  }
  if (flatfield)
  {
    *img /= *flatfield->getImage();
    *img *= mean;
    log(img.get(),"Divided flatfield '"+flatfield->getImage()->getName()+"'");
  }
  return img;
}

