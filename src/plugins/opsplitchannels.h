/********************************************************************************
 *                                                                              *
 * FitsIP - split channels of a multilayer image                                *
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

#ifndef OPSPLITCHANNELS_H
#define OPSPLITCHANNELS_H

#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpSplitChannels: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpSplitChannels();
  virtual ~OpSplitChannels();

  virtual bool createsNewImage() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  std::vector<std::shared_ptr<FitsImage>> split(std::shared_ptr<FitsImage> image);

private:
  std::vector<std::shared_ptr<FitsImage>> images;
};

#endif // OPSPLITCHANNELS_H
