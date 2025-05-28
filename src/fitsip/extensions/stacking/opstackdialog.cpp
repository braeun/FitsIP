/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to stack images                                              *
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

#include "opstackdialog.h"
#include "ui_opstackdialog.h"

OpStackDialog::OpStackDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpStackDialog)
{
  ui->setupUi(this);
}

OpStackDialog::~OpStackDialog()
{
  delete ui;
}

int OpStackDialog::getAlignment() const
{
  return ui->modeBox->currentIndex();
}

bool OpStackDialog::isSubtractSky() const
{
  return ui->subtractSkyBox->isChecked();
}

bool OpStackDialog::isFullTemplateMatch() const
{
  return ui->matchFullBox->isChecked();
}

int OpStackDialog::getTemplateMatchRange() const
{
  return ui->matchRangeField->text().toInt();
}

bool OpStackDialog::isAllowRotation() const
{
  return ui->allowRotationBox->isChecked();
}

int OpStackDialog::getStarBoxSize() const
{
  return ui->starboxSizeField->text().toInt();
}

int OpStackDialog::getSearchBoxSize() const
{
  return ui->searchboxSizeField->text().toInt();
}

int OpStackDialog::getStarMaxMovement() const
{
  return ui->maxMovementField->text().toInt();
}

