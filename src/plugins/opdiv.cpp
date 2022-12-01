/********************************************************************************
 *                                                                              *
 * FitsIP - divide two images                                                   *
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

#include "opdiv.h"
#include <imagecollection.h>
#include <fileobject.h>
#include <math.h>
#include <QDebug>

OpDiv::OpDiv():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpDiv");
}

OpDiv::~OpDiv()
{
  if (dlg) dlg->deleteLater();
}

QString OpDiv::getMenuEntry() const
{
  return "Math/Div...";
}

OpPlugin::ResultType OpDiv::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/)
{
  if (dlg == nullptr)
  {
    dlg = new ImageSelectDialog();
    dlg->setTitle("Divide by Image");
    dlg->setImageCollection(&ImageCollection::getGlobal());
  }
  if (dlg->exec())
  {
    std::shared_ptr<FileObject> file = dlg->getImage();
    profiler.start();
    try
    {
      *image /= *file->getImage();
      profiler.stop();
      log(image,"Divided by image "+file->getImage()->getName());
      logProfiler(image);
    }
    catch (const std::exception& ex)
    {
      setError(ex.what());
      qWarning() << ex.what();
      return ERROR;
    }
    return OK;
  }
  return CANCELLED;
}

