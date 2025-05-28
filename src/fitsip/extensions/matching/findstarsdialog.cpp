/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for star detection                                           *
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

#include "findstarsdialog.h"
#include "ui_findstarsdialog.h"

FindStarsDialog::FindStarsDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::FindStarsDialog)
{
  ui->setupUi(this);
}

FindStarsDialog::~FindStarsDialog()
{
  delete ui;
}

double FindStarsDialog::getSkyMean() const
{
  return ui->skyMeanField->text().toDouble();
}

void FindStarsDialog::setSkyMean(double v)
{
  ui->skyMeanField->setText(QString::number(v));
}

double FindStarsDialog::getSkySigma() const
{
  return ui->skySigmaField->text().toDouble();
}

void FindStarsDialog::setSkySigma(double v)
{
  ui->skySigmaField->setText(QString::number(v));
}

double FindStarsDialog::getSkyMinSigma() const
{
  return ui->skyMinSigmaField->text().toDouble();
}

void FindStarsDialog::setSkyMinSigma(double v)
{
  ui->skyMinSigmaField->setText(QString::number(v));
}

double FindStarsDialog::getFWHMMin() const
{
  return ui->fwhmMinField->text().toDouble();
}

void FindStarsDialog::setFWHMMin(double v)
{
  ui->fwhmMinField->setText(QString::number(v));
}

double FindStarsDialog::getFWHMMax() const
{
  return ui->fwhmMaxField->text().toDouble();
}

void FindStarsDialog::setFWHMMax(double v)
{
  ui->fwhmMaxField->setText(QString::number(v));
}

double FindStarsDialog::getRoundnessMin() const
{
  return ui->roundnessMinField->text().toDouble();
}

void FindStarsDialog::setRoundnessMin(double v)
{
  ui->roundnessMinField->setText(QString::number(v));
}

double FindStarsDialog::getRoundnessMax() const
{
  return ui->roundnessMaxField->text().toDouble();
}

void FindStarsDialog::setRoundnessMax(double v)
{
  ui->roundnessMaxField->setText(QString::number(v));
}

double FindStarsDialog::getSharpnessMin() const
{
  return ui->sharpnessMinField->text().toDouble();
}

void FindStarsDialog::setSharpnessMin(double v)
{
  ui->sharpnessMinField->setText(QString::number(v));
}

double FindStarsDialog::getSharpnessMax() const
{
  return ui->sharpnessMaxField->text().toDouble();
}

void FindStarsDialog::setSharpnessMax(double v)
{
  ui->sharpnessMaxField->setText(QString::number(v));
}

double FindStarsDialog::getHotnessMin() const
{
  return ui->hotnessMinField->text().toDouble();
}

void FindStarsDialog::setHotnessMin(double v)
{
  ui->hotnessMinField->setText(QString::number(v));
}

double FindStarsDialog::getHotnessMax() const
{
  return ui->hotnessMaxField->text().toDouble();
}

void FindStarsDialog::setHotnessMax(double v)
{
  ui->hotnessMaxField->setText(QString::number(v));
}

int FindStarsDialog::getTinyboxSize() const
{
  return ui->boxSizeBox->value();
}

void FindStarsDialog::setTinyboxSize(int v)
{
  ui->boxSizeBox->setValue(v);
}

double FindStarsDialog::getMoveMax() const
{
  return ui->moveMaxField->text().toDouble();
}

void FindStarsDialog::setMoveMax(double v)
{
  ui->moveMaxField->setText(QString::number(v));
}

bool FindStarsDialog::isBlur() const
{
  return ui->blurBox->isChecked();
}

void FindStarsDialog::setBlur(bool flag)
{
  ui->blurBox->setChecked(flag);
}

int FindStarsDialog::getIterations() const
{
  return ui->iterationBox->value();
}

void FindStarsDialog::setIterations(int v)
{
  ui->iterationBox->setValue(v);
}

