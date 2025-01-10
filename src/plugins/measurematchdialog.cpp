/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to match two images                               *
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

#include "measurematchdialog.h"
#include "ui_measurematchdialog.h"

MeasureMatchDialog::MeasureMatchDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::MeasureMatchDialog)
{
  ui->setupUi(this);
}

MeasureMatchDialog::~MeasureMatchDialog()
{
  delete ui;
}

void MeasureMatchDialog::setImageCollection(ImageCollection* c)
{
  ui->imageSelectWidget->setImageCollection(c);
}

std::shared_ptr<FitsObject> MeasureMatchDialog::getImage()
{
  return ui->imageSelectWidget->getImage();
}

bool MeasureMatchDialog::isMatchFullImage() const
{
  return ui->matchFullBox->isChecked();
}

int32_t MeasureMatchDialog::getMatchRange() const
{
  return ui->matchRangeBox->value();
}

int32_t MeasureMatchDialog::getScaleFactor() const
{
  return ui->factorBox->value();
}

int32_t MeasureMatchDialog::getFirstPassDelta() const
{
  return ui->firstPassBox->value();
}

int32_t MeasureMatchDialog::getSubsample() const
{
  return ui->subsampleBox->value();
}
