/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create a test image fom a PSF                             *
 *                                                                              *
 * modified: 2023-02-03                                                         *
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

#include "psftestimagedialog.h"
#include "ui_psftestimagedialog.h"

PSFTestImageDialog::PSFTestImageDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PSFTestImageDialog)
{
  ui->setupUi(this);
}

PSFTestImageDialog::~PSFTestImageDialog()
{
  delete ui;
}

QString PSFTestImageDialog::getFunction() const
{
  return ui->psfWidget->getFunction();
}

std::vector<ValueType> PSFTestImageDialog::getParameters() const
{
  return ui->psfWidget->getParameters();
}

int PSFTestImageDialog::getWidth() const
{
  return ui->widthBox->value();
}

int PSFTestImageDialog::getHeight() const
{
  return  ui->heightBox->value();
}

double PSFTestImageDialog::getAmplitude() const
{
  return ui->amplitudeField->text().toDouble();
}

double PSFTestImageDialog::getCenterX() const
{
  return ui->centerXField->text().toDouble();
}

double PSFTestImageDialog::getCenterY() const
{
  return ui->centerYField->text().toDouble();
}

