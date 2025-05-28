/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to shift image with subpixel accuracy                        *
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

#include "opshiftdialog.h"
#include "ui_opshiftdialog.h"

OpShiftDialog::OpShiftDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpShiftDialog),
  imgWidth(0),
  imgHeight(0)
{
  ui->setupUi(this);
}

OpShiftDialog::~OpShiftDialog()
{
  delete ui;
}

void OpShiftDialog::setImageSize(int32_t w, int32_t h)
{
  if (w != imgWidth || h != imgHeight)
  {
    ui->xSpinBox->setRange(-w/2,w/2);
    ui->xSpinBox->setValue(0);
    ui->ySpinBox->setRange(-h/2,h/2);
    ui->ySpinBox->setValue(0);
    imgWidth = w;
    imgHeight = h;
  }
}

double OpShiftDialog::getDeltaX() const
{
  return ui->xSpinBox->value();
}

double OpShiftDialog::getDeltaY() const
{
  return ui->ySpinBox->value();
}
