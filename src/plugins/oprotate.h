/********************************************************************************
 *                                                                              *
 * FitsIP - rotate images                                                       *
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

#ifndef OPROTATE_H
#define OPROTATE_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpRotateDialog;

class OpRotate: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpRotate();
  virtual ~OpRotate() override;

  virtual QString getMenuEntry() const override;

  virtual QIcon getIcon() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

  void rotate90cw(std::shared_ptr<FitsImage> image) const;

  void rotate90ccw(std::shared_ptr<FitsImage> image) const;

  /**
   * @brief Rotate the image by an arbitrary angle
   * @param image the image to rotate
   * @param angle the rotation angle in degrees
   * @param crop if true, crop the image to the original size
   */
  void rotate(std::shared_ptr<FitsImage> image, ValueType angle, bool crop) const;

private:

  OpRotateDialog* dlg;

};

#endif // OPROTATE_H
