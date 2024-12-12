/********************************************************************************
 *                                                                              *
 * FitsIP - point spread function creation dialog                               *
 *                                                                              *
 * modified: 2022-11-25                                                         *
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

#include "psfdialog.h"
#include "ui_psfdialog.h"
#include <QDebug>

PSFDialog::PSFDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PSFDialog)
{
  ui->setupUi(this);
}

PSFDialog::~PSFDialog()
{
  delete ui;
}

QString PSFDialog::getFunction() const
{
  return ui->psfWidget->getFunction();
}

std::vector<ValueType> PSFDialog::getParameters() const
{
  return ui->psfWidget->getParameters();
}

int PSFDialog::getWidth() const
{
  return ui->imgWidthBox->value();
}

int PSFDialog::getHeight() const
{
  return ui->imgHeightBox->value();
}
