/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to crop image                                                *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#include "opcropdialog.h"
#include "ui_opcropdialog.h"

OpCropDialog::OpCropDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpCropDialog)
{
  ui->setupUi(this);
}

OpCropDialog::~OpCropDialog()
{
  delete ui;
}

bool OpCropDialog::isManual() const
{
  return ui->manualButton->isChecked();
}

void OpCropDialog::setSelection(QRect r)
{
  ui->manualButton->setChecked(true);
  ui->xField->setText(QString::number(r.x()));
  ui->yField->setText(QString::number(r.y()));
  ui->widthField->setText(QString::number(r.width()));
  ui->heightField->setText(QString::number(r.height()));
}

QRect OpCropDialog::getSelection() const
{
  int32_t x = ui->xField->text().toInt();
  int32_t y = ui->yField->text().toInt();
  int32_t w = ui->widthField->text().toInt();
  int32_t h = ui->heightField->text().toInt();
  return QRect(x,y,w,h);
}

ValueType OpCropDialog::getThreshold() const
{
  return static_cast<ValueType>(ui->thresholdField->text().toDouble());
}

int OpCropDialog::getBorder() const
{
  return ui->borderField->text().toInt();
}
