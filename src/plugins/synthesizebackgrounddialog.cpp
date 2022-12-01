/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to synthesize background from image                          *
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

#include "synthesizebackgrounddialog.h"
#include "ui_synthesizebackgrounddialog.h"

SynthesizeBackgroundDialog::SynthesizeBackgroundDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SynthesizeBackgroundDialog)
{
  ui->setupUi(this);
}

SynthesizeBackgroundDialog::~SynthesizeBackgroundDialog()
{
  delete ui;
}

void SynthesizeBackgroundDialog::setSky(AverageResult avg)
{
  double sky;
  double skysig;
  std::tie(std::ignore,sky,skysig,std::ignore) = avg;
  ui->skyMeanField->setText(QString::number(sky));
  ui->skySigmaField->setText(QString::number(skysig));
}

uint32_t SynthesizeBackgroundDialog::getSelectionMode() const
{
  return ui->selectionBox->currentIndex();
}

uint32_t SynthesizeBackgroundDialog::getPointsCount() const
{
  return ui->pointSpinner->value();
}

uint32_t SynthesizeBackgroundDialog::getPolynomDegree() const
{
  return ui->degreeSpinner->value();
}

double SynthesizeBackgroundDialog::getBackground() const
{
  return ui->skyMeanField->text().toDouble() +
      ui->skySigmaField->text().toDouble() * ui->skyNSigmaField->text().toDouble();
}
