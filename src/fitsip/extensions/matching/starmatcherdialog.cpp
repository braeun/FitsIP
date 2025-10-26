/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to match two images by stars                      *
 *                                                                              *
 * modified: 2025-10-26                                                         *
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

#include "starmatcherdialog.h"
#include "ui_starmatcherdialog.h"
#include <fitsip/core/widgets/imageselectwidget.h>

StarMatcherDialog::StarMatcherDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::StarMatcherDialog)
{
  ui->setupUi(this);
  imageSelectWidget = new ImageSelectWidget(ui->groupBox);
  imageSelectWidget->setObjectName(QString::fromUtf8("imageSelectWidget"));
  ui->verticalLayout_2->addWidget(imageSelectWidget);
}

StarMatcherDialog::~StarMatcherDialog()
{
  delete ui;
}

void StarMatcherDialog::setImageCollection(ImageCollection* c)
{
  imageSelectWidget->setImageCollection(c);
}

std::shared_ptr<FitsObject> StarMatcherDialog::getImage()
{
  return imageSelectWidget->getImage();
}

bool StarMatcherDialog::isSubtractSky() const
{
  return ui->subtractSkyBox->isChecked();
}

bool StarMatcherDialog::isAllowRotation() const
{
  return ui->allowRotationBox->isChecked();
}

int StarMatcherDialog::getStarBoxSize() const
{
  return ui->starboxSizeField->text().toInt();
}

int StarMatcherDialog::getSearchBoxSize() const
{
  return ui->searchboxSizeField->text().toInt();
}

int StarMatcherDialog::getStarMaxMovement() const
{
  return ui->maxMovementField->text().toInt();
}

