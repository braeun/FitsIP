/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to align images                                   *
 *                                                                              *
 * modified: 2022-11-22                                                         *
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
#include <QFileDialog>

OpAlignDialog::OpAlignDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpAlignDialog),
  outputPath(".")
{
  ui->setupUi(this);
  ui->outputDirField->setText(outputPath);
}

OpAlignDialog::~OpAlignDialog()
{
  delete ui;
}

void OpAlignDialog::setOutputPath(const QString &p)
{
  ui->outputDirField->setText(p);
}

QString OpAlignDialog::getOutputPath() const
{
  return ui->outputDirField->text();
}


void OpAlignDialog::on_browseButton_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this,QApplication::applicationDisplayName(),ui->outputDirField->text());
  if (!dir.isEmpty())
  {
    ui->outputDirField->setText(dir);
  }
}
