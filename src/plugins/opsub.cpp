/********************************************************************************
 *                                                                              *
 * FitsIP - subtract to images                                                  *
 *                                                                              *
 * modified: 2023-02-04                                                         *
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

#include "opsub.h"
#include <fitsbase/imagecollection.h>
#include <fitsbase/fileobject.h>
#include <fitsbase/fitsimage.h>
#include <QDebug>

OpSub::OpSub():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpSub");
}

OpSub::~OpSub()
{
  if (dlg) dlg->deleteLater();
}

QString OpSub::getMenuEntry() const
{
  return "Math/Sub...";
}

QIcon OpSub::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/minus.png");
}

OpPlugin::ResultType OpSub::execute(std::shared_ptr<FitsImage> image, QRect /*selection*/, const PreviewOptions& opt)
{
  if (dlg == nullptr)
  {
    dlg = new ImageSelectDialog();
    dlg->setTitle("Subtract by Image");
    dlg->setImageCollection(&ImageCollection::getGlobal());
  }
  dlg->setPrompt("Subtract from "+image->getName()+":");
  if (dlg->exec())
  {
    std::shared_ptr<FileObject> file = dlg->getImage();
    profiler.start();
    try
    {
      *image -= *file->getImage();
      profiler.stop();
      log(image,"Subtracted image "+file->getImage()->getName());
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

