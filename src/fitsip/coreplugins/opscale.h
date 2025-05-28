/********************************************************************************
 *                                                                              *
 * FitsIP - linear scaling of image intensity                                   *
 *                                                                              *
 * modified: 2025-01-31                                                         *
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

#ifndef OPSCALE_H
#define OPSCALE_H


#include <fitsip/core/opplugin.h>
#include <fitsip/core/dialogs/twovaluedialog.h>
#include <QObject>

class OpScale: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)

public:
  OpScale();
  virtual ~OpScale() override;

  virtual QString getMenuEntry() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

private:
  void scaleImage(std::shared_ptr<FitsImage> img, ValueType scale, ValueType bias) const;

  TwoValueDialog dlg;

};

#endif // OPSCALE_H
