/********************************************************************************
 *                                                                              *
 * FitsIP - widget to select an image from memory or filesystem                 *
 *                                                                              *
 * modified: 2024-12-13                                                         *
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

#ifndef IMAGESELECTWIDGET_H
#define IMAGESELECTWIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class ImageSelectWidget;
}

class ImageCollection;
class FileObject;

class ImageSelectWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ImageSelectWidget(QWidget *parent = nullptr);
  ~ImageSelectWidget();

  void setNoneAllowed(bool flag);

  void setImageCollection(ImageCollection* collection);

  std::shared_ptr<FileObject> getImage();

private slots:
  void on_browseButton_clicked();

private:
  Ui::ImageSelectWidget *ui;
  ImageCollection* collection;
};

#endif // IMAGESELECTWIDGET_H
