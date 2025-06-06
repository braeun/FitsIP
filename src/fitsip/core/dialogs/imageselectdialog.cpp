/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to select an image from memory or filesystem                 *
 *                                                                              *
 * modified: 2025-06-06                                                         *
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
#include "../fitsobject.h"
#include "../widgets/imageselectwidget.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

ImageSelectDialog::ImageSelectDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ImageSelectDialog),
  collection(nullptr)
{
  ui->setupUi(this);
  imageSelectWidget = new ImageSelectWidget(this);
  imageSelectWidget->setObjectName(QString::fromUtf8("imageSelectWidget"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(1);
  sizePolicy.setHeightForWidth(imageSelectWidget->sizePolicy().hasHeightForWidth());
  imageSelectWidget->setSizePolicy(sizePolicy);
  ui->verticalLayout->insertWidget(ui->verticalLayout->indexOf(ui->buttonBox),imageSelectWidget);
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
  imageSelectWidget->setImageCollection(c);
}

std::shared_ptr<FitsObject> ImageSelectDialog::getImage()
{
  return imageSelectWidget->getImage();
}
