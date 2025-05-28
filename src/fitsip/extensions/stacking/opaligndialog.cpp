/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to align images                                   *
 *                                                                              *
 * modified: 2022-12-03                                                         *
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

#include "opaligndialog.h"
#include "ui_opaligndialog.h"
#include <fitsip/core/settings.h>
#include <QFileDialog>

static const char* PATH_OPALIGN = "fits/path/opalign";

OpAlignDialog::OpAlignDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpAlignDialog)
{
  ui->setupUi(this);
  ui->outputDirField->setText(Settings().getString(PATH_OPALIGN,"."));
}

OpAlignDialog::~OpAlignDialog()
{
  delete ui;
}

QString OpAlignDialog::getOutputPath() const
{
  return ui->outputDirField->text();
}

bool OpAlignDialog::isMatchFull() const
{
  return ui->matchFullButton->isChecked();
}

int OpAlignDialog::getMatchRange() const
{
  return ui->matchRangeBox->value();
}

bool OpAlignDialog::isAdaptAOI() const
{
  return ui->dynamicAOIBox->isChecked();
}




void OpAlignDialog::on_browseButton_clicked()
{
  QString dir = Settings().getExistingDirectory(this,PATH_OPALIGN);
  if (!dir.isEmpty())
  {
    ui->outputDirField->setText(dir);
  }
}
