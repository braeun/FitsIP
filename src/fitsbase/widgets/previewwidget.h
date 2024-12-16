/********************************************************************************
 *                                                                              *
 * FitsIP - widgets for previews in dialogs                                     *
 *                                                                              *
 * modified: 2024-12-16                                                         *
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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include "previewoptions.h"
#include <memory>
#include <QWidget>

class FitsImage;

namespace Ui {
class PreviewWidget;
}

class PreviewWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PreviewWidget(QWidget *parent = nullptr);
  ~PreviewWidget();

  void setOptions(const PreviewOptions& opt);

  void setSourceImage(std::shared_ptr<FitsImage> image, QRect selection);

  std::shared_ptr<FitsImage> getSourceImage() const;

  void updatePreview(std::shared_ptr<FitsImage> image);

private:
  Ui::PreviewWidget *ui;
  std::shared_ptr<FitsImage> sourceImage;
  PreviewOptions options;
};

#endif // PREVIEWWIDGET_H
