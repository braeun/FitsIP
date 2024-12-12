/********************************************************************************
 *                                                                              *
 * FitsIP - simple dialog to enter two values                                   *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#include "twovaluedialog.h"
#include "ui_twovaluedialog.h"

TwoValueDialog::TwoValueDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::TwoValueDialog)
{
  ui->setupUi(this);
}

TwoValueDialog::~TwoValueDialog()
{
  delete ui;
}

void TwoValueDialog::setTitle(QString title)
{
  ui->titleLabel->setText(title);
}

void TwoValueDialog::setValue1Label(QString label, QString unit)
{
  ui->value1Label->setText(label);
  ui->value1UnitLabel->setText(unit);
}

void TwoValueDialog::setValue2Label(QString label, QString unit)
{
  ui->value2Label->setText(label);
  ui->value2UnitLabel->setText(unit);
}

QString TwoValueDialog::getValue1() const
{
  return ui->value1Field->text();
}

void TwoValueDialog::setValue1(QString v)
{
  ui->value1Field->setText(v);
}


QString TwoValueDialog::getValue2() const
{
  return ui->value2Field->text();
}

void TwoValueDialog::setValue2(QString v)
{
  ui->value2Field->setText(v);
}


