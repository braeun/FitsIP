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

#ifndef MEASUREDISTANCE_H
#define MEASUREDISTANCE_H

#include <fitsip/core/opplugin.h>
#include <QObject>
#include <vector>

class MeasureDistanceDialog;

class MeasureDistance: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  MeasureDistance();
  virtual ~MeasureDistance() override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

private:
  MeasureDistanceDialog* dlg;
};

#endif // MEASUREDISTANCE_H
