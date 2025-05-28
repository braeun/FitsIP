/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create box test image                                     *
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

#include "boxtestimagedialog.h"
#include "ui_boxtestimagedialog.h"

BoxTestImageDialog::BoxTestImageDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::BoxTestImageDialog)
{
  ui->setupUi(this);
}

BoxTestImageDialog::~BoxTestImageDialog()
{
  delete ui;
}

int BoxTestImageDialog::getWidth() const
{
  return ui->widthBox->value();
}

int BoxTestImageDialog::getHeight() const
{
  return ui->heightBox->value();
}

double BoxTestImageDialog::getAmplitude() const
{
  return ui->amplitudeField->text().toDouble();
}

int BoxTestImageDialog::getCenterX() const
{
  return ui->centerXBox->value();
}

int BoxTestImageDialog::getBoxWidth() const
{
  return ui->boxWidthBox->value();
}

int BoxTestImageDialog::getCenterY() const
{
  return ui->centerYBox->value();
}

int BoxTestImageDialog::getBoxHeight() const
{
  return ui->boxHeightBox->value();
}

