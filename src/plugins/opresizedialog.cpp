/********************************************************************************
 *                                                                              *
 * FitsIP - resize image dialog                                                 *
 *                                                                              *
 * modified: 2025-02-26                                                         *
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

#include "opresizedialog.h"
#include "ui_opresizedialog.h"

OpResizeDialog::OpResizeDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpResizeDialog),
  currentWidth(100),
  currentHeight(100)
{
  ui->setupUi(this);
  connect(ui->keepAspectRatioBox,&QCheckBox::clicked,this,[this](bool checked){ui->yFactorSpinner->setDisabled(checked);});
}

OpResizeDialog::~OpResizeDialog()
{
  delete ui;
}

void OpResizeDialog::setCurrentSize(int w, int h)
{
  currentWidth = w;
  currentHeight = h;
  ui->widthField->setText(QString::number(currentWidth));
  ui->heightField->setText(QString::number(currentHeight));
}



double OpResizeDialog::getFactorX() const
{
  if (ui->factorButton->isChecked())
  {
    return ui->xFactorSpinner->value();
  }
  if (ui->widthField->text().isEmpty())
  {
    return 1.0;
  }
  double f = ui->widthField->text().toDouble() / currentWidth;
  if (ui->keepAspectRatioBox->isChecked() && !ui->heightField->text().isEmpty())
  {
    double f1 = ui->heightField->text().toDouble() / currentHeight;
    f = std::min(f,f1);
  }
  return f;
}

double OpResizeDialog::getFactorY() const
{
  if (ui->keepAspectRatioBox->isChecked()) return getFactorX();
  if (ui->factorButton->isChecked())
  {
    return ui->yFactorSpinner->value();
  }
  return ui->heightField->text().toDouble() / currentHeight;
}

bool OpResizeDialog::isNoScaling() const
{
  return ui->noScalingButton->isChecked();
}

bool OpResizeDialog::isNearestNeighborInterpolation() const
{
  return ui->nearestNeighborButton->isChecked();
}

bool OpResizeDialog::isBilinearInterpolation() const
{
  return ui->bilinearButton->isChecked();
}

