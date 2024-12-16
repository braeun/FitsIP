/********************************************************************************
 *                                                                              *
 * FitsIP - enlarge image                                                       *
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

#ifndef OPGROW_H
#define OPGROW_H

#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpGrowDialog;

class OpGrow: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpGrow();
  virtual ~OpGrow() override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  std::shared_ptr<FitsImage> grow(std::shared_ptr<FitsImage> image, uint32_t factor, bool bilinear);

private:
  OpGrowDialog* dlg;
};

#endif // OPGROW_H
