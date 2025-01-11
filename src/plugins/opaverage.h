/********************************************************************************
 *                                                                              *
 * FitsIP - create an average of several images                                 *
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

#ifndef OPAVERAGE_H
#define OPAVERAGE_H

#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpAverage:  public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)
public:
  OpAverage();
  virtual ~OpAverage() override;

  virtual bool createsNewImage() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

  virtual ResultType execute(const std::vector<QFileInfo>& list, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

private:
  ResultType add(const QFileInfo& file);
  std::shared_ptr<FitsImage> img;

};

#endif // OPAVERAGE_H
