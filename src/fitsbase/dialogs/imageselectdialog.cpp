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

#include "imageselectdialog.h"
#include "ui_imageselectdialog.h"
#include "../imagecollection.h"
#include "../fileobject.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

ImageSelectDialog::ImageSelectDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ImageSelectDialog),
  collection(nullptr)
{
  ui->setupUi(this);
}

ImageSelectDialog::~ImageSelectDialog()
{
  delete ui;
}

void ImageSelectDialog::setTitle(QString title)
{
  ui->titleLabel->setText(title);
}

void ImageSelectDialog::setPrompt(QString txt)
{
  ui->promptLabel->setText(txt);
}

void ImageSelectDialog::setImageCollection(ImageCollection* c)
{
  ui->imageSelectWidget->setImageCollection(c);
}

std::shared_ptr<FileObject> ImageSelectDialog::getImage()
{
  return ui->imageSelectWidget->getImage();
}
