/********************************************************************************
 *                                                                              *
 * FitsIP - Lucy Richardson deconvolution dialog                                *
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

#include "lucyrichardsondeconvolutiondialog.h"
#include "ui_lucyrichardsondeconvolutiondialog.h"

LucyRichardsonDeconvolutionDialog::LucyRichardsonDeconvolutionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LucyRichardsonDeconvolutionDialog)
{
  ui->setupUi(this);
}

LucyRichardsonDeconvolutionDialog::~LucyRichardsonDeconvolutionDialog()
{
  delete ui;
}

QString LucyRichardsonDeconvolutionDialog::getFunction() const
{
  return ui->psfWidget->getFunction();
}

std::vector<ValueType> LucyRichardsonDeconvolutionDialog::getParameters() const
{
  return ui->psfWidget->getParameters();
}

int LucyRichardsonDeconvolutionDialog::getIterationCount() const
{
  return ui->iterationsBox->value();
}

bool LucyRichardsonDeconvolutionDialog::isCutImage() const
{
  return ui->cutBox->isChecked();
}

bool LucyRichardsonDeconvolutionDialog::isSineFunction() const
{
  return ui->sineRelaxationBox->isChecked();
}

bool LucyRichardsonDeconvolutionDialog::isConstMultFunction() const
{
  return ui->valueRelaxationBox->isChecked();
}

ValueType LucyRichardsonDeconvolutionDialog::getParameter() const
{
  if (isSineFunction())
    return ui->sineValueBox->value();
  return ui->singleValueBox->value();
}

