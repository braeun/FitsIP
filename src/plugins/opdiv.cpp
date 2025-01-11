/********************************************************************************
 *                                                                              *
 * FitsIP - divide two images                                                   *
 *                                                                              *
 * modified: 2024-12-13                                                         *
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
#include <fitsbase/imagecollection.h>
#include <fitsbase/fitsobject.h>
#include <cmath>
#include <QDebug>

OpDiv::OpDiv():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
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

QIcon OpDiv::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/div.png");
}

OpPlugin::ResultType OpDiv::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  if (dlg == nullptr)
  {
    dlg = new ImageSelectDialog();
    dlg->setTitle("Divide by Image");
    dlg->setImageCollection(&ImageCollection::getGlobal());
  }
  dlg->setPrompt("Divide "+image->getName()+ "by:");
  if (dlg->exec())
  {
    std::shared_ptr<FitsObject> file = dlg->getImage();
    profiler.start();
    try
    {
      *image->getImage() /= *file->getImage();
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

