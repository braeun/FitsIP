/********************************************************************************
 *                                                                              *
 * FitsIP - resize image dialog                                                 *
 *                                                                              *
 * modified: 2023-02-04                                                         *
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

OpResizeDialog::OpResizeDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpResizeDialog)
{
  ui->setupUi(this);
}

OpResizeDialog::~OpResizeDialog()
{
  delete ui;
}

int OpResizeDialog::getMode()
{
  return ui->modeBox->currentIndex();
}

int OpResizeDialog::getFactor()
{
  return ui->factorBox->value();
}

bool OpResizeDialog::isNearestNeighborInterpolation()
{
  return ui->nearestNeighborButton->isChecked();
}

bool OpResizeDialog::isBilinearInterpolation()
{
  return ui->bilinearButton->isChecked();
}

