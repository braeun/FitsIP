/********************************************************************************
 *                                                                              *
 * FitsIP - image calibration with flatfield and dark image                     *
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

#ifndef OPCALIBRATION_H
#define OPCALIBRATION_H

#include <fitsbase/opplugin.h>
#include <QObject>
#include <memory>

#define QT_STATICPLUGIN
#include <QtPlugin>

class FitsObject;
class OpCalibrationDialog;

class OpCalibration: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpCalibration();
  virtual ~OpCalibration() override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

  virtual ResultType execute(const std::vector<QFileInfo>& list, const OpPluginData& data=OpPluginData()) override;

private:
  std::shared_ptr<FitsImage> calibrate(const QFileInfo& info, std::shared_ptr<FitsObject> darkframe, std::shared_ptr<FitsObject> flatfield, double mean);

  OpCalibrationDialog* dlg;
};

#endif // OPCALIBRATION_H
