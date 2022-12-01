/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create gaussian test image                                *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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

#include "gaussiantestimagedialog.h"
#include "ui_gaussiantestimagedialog.h"

GaussianTestImageDialog::GaussianTestImageDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::GaussianTestImageDialog)
{
  ui->setupUi(this);
}

GaussianTestImageDialog::~GaussianTestImageDialog()
{
  delete ui;
}

int32_t GaussianTestImageDialog::getWidth() const
{
  return ui->widthBox->value();
}

int32_t GaussianTestImageDialog::getHeight() const
{
  return  ui->heightBox->value();
}

double GaussianTestImageDialog::getAmplitude() const
{
  return ui->amplitudeField->text().toDouble();
}

double GaussianTestImageDialog::getCenterX() const
{
  return ui->centerXField->text().toDouble();
}

double GaussianTestImageDialog::getSigmaX() const
{
  return ui->sigmaXField->text().toDouble();
}

double GaussianTestImageDialog::getCenterY() const
{
  return ui->centerYField->text().toDouble();
}

double GaussianTestImageDialog::getSigmaY() const
{
  return ui->sigmaYField->text().toDouble();
}

