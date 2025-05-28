/********************************************************************************
 *                                                                              *
 * FitsIP - create box test image                                               *
 *                                                                              *
 * modified: 2025-05-24                                                         *
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

#ifndef BOXTESTIMAGE_H
#define BOXTESTIMAGE_H

#include <fitsip/core/opplugin.h>
#include <QObject>

class BoxTestImageDialog;

class BoxTestImage: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  BoxTestImage();
  virtual ~BoxTestImage() override;

  virtual bool requiresImage() const override;

  virtual bool requiresFileList() const override;

  virtual bool createsNewImage() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

private:
  std::shared_ptr<FitsImage> img;
  BoxTestImageDialog* dlg;
};

#endif // BOXTESTIMAGE_H
