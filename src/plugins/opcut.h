/********************************************************************************
 *                                                                              *
 * FitsIP - cut low and high values                                             *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#ifndef OPCUT_H
#define OPCUT_H

#include <fitstypes.h>
#include <opplugin.h>
#include <dialogs/twovaluedialog.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpCut: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpCut();
  virtual ~OpCut() override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> image, QRect selection=QRect()) override;

  void cut(std::shared_ptr<FitsImage> image, ValueType lower=0, ValueType upper=std::numeric_limits<ValueType>::max());

private:
  TwoValueDialog dlg;

};

#endif // OPCUT_H
