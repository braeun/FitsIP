/********************************************************************************
 *                                                                              *
 * FitsIP - shift image with subpixel accuracy                                  *
 *                                                                              *
 * modified: 2025-02-06                                                         *
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

#ifndef OPSHIFT_H
#define OPSHIFT_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpShiftDialog;

class OpShift: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpShift();
  virtual ~OpShift() override;

  virtual QString getMenuEntry() const override;

  virtual QIcon getIcon() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

  void shift(std::shared_ptr<FitsImage> image, ValueType dx, ValueType dy) const;

private:
  OpShiftDialog* dlg;
};

#endif // OPSHIFT_H
