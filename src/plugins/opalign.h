/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to align images                                              *
 *                                                                              *
 * modified: 2022-12-03                                                         *
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

#ifndef OPALIGN_H
#define OPALIGN_H

#include "measurematch.h"
#include <opplugin.h>
#include <dialogs/imageselectdialog.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpAlignDialog;

class OpAlign: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)
public:
  OpAlign();
  virtual ~OpAlign() override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

  virtual ResultType execute(const std::vector<QFileInfo>& list, QRect selection=QRect()) override;

private:
  ResultType prepare(const QFileInfo& file, QRect aoi);
  ResultType align(const QFileInfo& file);
//  ResultType save(std::shared_ptr<FitsImage> image, const QFileInfo& info);

  OpAlignDialog* dlg;
  QString outputPath;
  std::shared_ptr<FitsImage> img;
  MeasureMatch matcher;
  bool matchFull;
  int matchRange;
  bool adaptAOI;
};

#endif // OPALIGN_H
