/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to shrink image                                              *
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

#include "opshrinkdialog.h"
#include "ui_opshrinkdialog.h"

OpShrinkDialog::OpShrinkDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpShrinkDialog)
{
  ui->setupUi(this);
}

OpShrinkDialog::~OpShrinkDialog()
{
  delete ui;
}

uint32_t OpShrinkDialog::getFactor()
{
  return static_cast<uint32_t>(ui->factorBox->value());
}
