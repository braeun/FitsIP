/********************************************************************************
 *                                                                              *
 * FitsIP - about dialog                                                        *
 *                                                                              *
 * modified: 2024-12-13                                                         *
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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <fitsip/core/fitsconfig.h>

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->versionLabel->setText("Version "+QApplication::applicationVersion());
  ui->releaseLabel->setText(QString("Release ")+__DATE__);
//   QString options = "Build Options:\n";
// #ifdef USE_FLOAT
//   options += "  Value Type      = float\n";
// #else
//   options += "  ValueType = double\n";
// #endif
// #ifdef USE_16BITRAW
//   options += "  Raw Image Input = 16 bits\n";
// #else
// #endif
//   ui->buildOptionLabel->setText(options);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
