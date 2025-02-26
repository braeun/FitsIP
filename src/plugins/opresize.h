/********************************************************************************
 *                                                                              *
 * FitsIP - resize image                                                        *
 *                                                                              *
 * modified: 2023-02-26                                                         *
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

#ifndef OPRESIZE_H
#define OPRESIZE_H

#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class OpResizeDialog;

class OpResize: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  OpResize();
  virtual ~OpResize() override;

  virtual QString getMenuEntry() const override;

  virtual QIcon getIcon() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  std::shared_ptr<FitsImage> resize(std::shared_ptr<FitsImage> image, double factorx, double factory, int mode) const;

//  std::shared_ptr<FitsImage> grow(std::shared_ptr<FitsImage> image, int factor, bool bilinear) const;

//  std::shared_ptr<FitsImage> shrink(std::shared_ptr<FitsImage> image, int factor) const;

  std::shared_ptr<FitsImage> shrink(std::shared_ptr<FitsImage> image, double factorx, double factory) const;

  std::shared_ptr<FitsImage> growNearestNeighbor(std::shared_ptr<FitsImage> image, double factorx, double factory) const;

  std::shared_ptr<FitsImage> growBilinear(std::shared_ptr<FitsImage> image, double factorx, double factory) const;

private:
  void scriptResize(std::shared_ptr<FitsObject> obj, double factorx, double factory, char mode) const;

  OpResizeDialog* dlg;
};


#endif // OPRESIZE_H
