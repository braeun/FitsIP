/********************************************************************************
 *                                                                              *
 * FitsIP - measure the distance of two points in an image                      *
 *                                                                              *
 * modified: 2025-03-11                                                         *
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

#include "measuredistance.h"
#include "measuredistancedialog.h"

MeasureDistance::MeasureDistance():
  dlg(nullptr)
{
}

MeasureDistance::~MeasureDistance()
{
  if (dlg) dlg->deleteLater();
}

QString MeasureDistance::getMenuEntry() const
{
  return "Analyse/Distance...";
}

OpPlugin::ResultType MeasureDistance::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg)
  {
    dlg = new MeasureDistanceDialog();
  }
  dlg->setPixelList(image->getPixelList());
  dlg->exec();
  return OpPlugin::OK;
}


