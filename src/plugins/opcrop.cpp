/********************************************************************************
 *                                                                              *
 * FitsIP - crop image                                                          *
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

#include "opcrop.h"
#include "opcropdialog.h"
#include <fitsbase/fitsimage.h>

OpCrop::OpCrop():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpCrop");
}

OpCrop::~OpCrop()
{
  if (dlg) dlg->deleteLater();
}

QString OpCrop::getMenuEntry() const
{
  return "Image/Crop...";
}

QIcon OpCrop::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/transform-crop.png");
}

OpPlugin::ResultType OpCrop::execute(std::shared_ptr<FitsImage> image, QRect r)
{
  if (!dlg) dlg = new OpCropDialog();
  dlg->setSelection(r);
  if (dlg->exec())
  {
    r = image->getOverlap(dlg->getSelection());
    if (r.isValid())
    {
      profiler.start();
      auto img = image->subImage(r);
      img->setMetadata(image->getMetadata());
      *image = *img;
      profiler.stop();
      log(image,QString("OpCrop: %1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
      logProfiler(image);
      return OK;
    }
  }
  return CANCELLED;
}
