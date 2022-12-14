/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to enlarge image                                             *
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

#include "opgrowdialog.h"
#include "ui_opgrowdialog.h"

OpGrowDialog::OpGrowDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpGrowDialog)
{
  ui->setupUi(this);
}

OpGrowDialog::~OpGrowDialog()
{
  delete ui;
}

uint32_t OpGrowDialog::getFactor()
{
  return static_cast<uint32_t>(ui->factorBox->value());
}

bool OpGrowDialog::isNearestNeighborInterpolation()
{
  return ui->nearestNeighborButton->isChecked();
}

bool OpGrowDialog::isBilinearInterpolation()
{
  return ui->bilinearButton->isChecked();
}
