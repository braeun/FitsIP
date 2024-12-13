/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to select an image from memory or filesystem                 *
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

#ifndef IMAGESELECTDIALOG_H
#define IMAGESELECTDIALOG_H

#include <QDialog>
#include <memory>

namespace Ui {
class ImageSelectDialog;
}

class ImageCollection;
class FileObject;

class ImageSelectDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ImageSelectDialog(QWidget *parent = nullptr);
  ~ImageSelectDialog();

  void setTitle(QString title);

  void setPrompt(QString txt);

  void setImageCollection(ImageCollection* collection);

  std::shared_ptr<FileObject> getImage();

private:
  Ui::ImageSelectDialog *ui;
  ImageCollection* collection;
};

#endif // IMAGESELECTDIALOG_H
