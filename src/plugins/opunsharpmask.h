/********************************************************************************
 *                                                                              *
 * FitsIP - unsharp masking                                                     *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#ifndef OPUNSHARPMASK_H
#define OPUNSHARPMASK_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpUnsharpMaskDialog;


class OpUnsharpMask: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpUnsharpMask();
  virtual ~OpUnsharpMask();

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  void unsharpmask(std::shared_ptr<FitsImage> image, ValueType sigma, ValueType strength);

private:
  OpUnsharpMaskDialog* dlg;
};

#endif // OPUNSHARPMASK_H
