/********************************************************************************
 *                                                                              *
 * FitsIP - scale image by logarithm                                            *
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

#ifndef OPLOG_H
#define OPLOG_H

#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>
class OpLog: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpLog();
  virtual ~OpLog() override;

  virtual QString getMenuEntry() const override;

  virtual QIcon getIcon() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

};

#endif // OPLOG_H
