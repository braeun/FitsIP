/********************************************************************************
 *                                                                              *
 * FitsIP - cut low and high values                                             *
 *                                                                              *
 * modified: 2025-05-25                                                         *
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

#include <fitsip/core/fitstypes.h>
#include <fitsip/core/opplugin.h>
#include <fitsip/core/dialogs/twovaluedialog.h>
#include <QObject>

class OpCut: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)

public:
  OpCut();
  virtual ~OpCut() override;

  virtual QString getMenuEntry() const override;

  virtual QIcon getIcon() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

private:
  TwoValueDialog dlg;

};

#endif // OPCUT_H
