/********************************************************************************
 *                                                                              *
 * FitsIP - Lucy Richardson deconvolution dialog                                *
 *                                                                              *
 * modified: 2025-05-29                                                         *
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
#include <fitsip/core/widgets/psfwidget.h>
#include <QDir>
#include <QFileDialog>
#include <QSettings>

static const char* path_setting = "lucyrichardson/path";

LucyRichardsonDeconvolutionDialog::LucyRichardsonDeconvolutionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LucyRichardsonDeconvolutionDialog)
{
  ui->setupUi(this);
  psfWidget = new PSFWidget(ui->groupBox);
  psfWidget->setObjectName(QString::fromUtf8("psfWidget"));
  ui->verticalLayout_2->addWidget(psfWidget);
  connect(ui->browseButton,&QPushButton::clicked,this,[this](){browse();});
  QSettings settings;
  QString path = settings.value(path_setting,QDir::homePath()).toString();
  ui->pathField->setText(path);
}

LucyRichardsonDeconvolutionDialog::~LucyRichardsonDeconvolutionDialog()
{
  delete ui;
}

void LucyRichardsonDeconvolutionDialog::updatePSFList()
{
  psfWidget->updatePSFList();
}

QString LucyRichardsonDeconvolutionDialog::getFunction() const
{
  return psfWidget->getFunction();
}

std::vector<ValueType> LucyRichardsonDeconvolutionDialog::getParameters() const
{
  return psfWidget->getParameters();
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

bool LucyRichardsonDeconvolutionDialog::isStoreIntermediate() const
{
  return ui->storeIntermediateBox->isChecked();
}

QString LucyRichardsonDeconvolutionDialog::getPath() const
{
  QString p = ui->pathField->text();
  QDir dir(p);
  if (!dir.exists())
  {
    dir.mkpath(p);
  }
  return p;
}

ValueType LucyRichardsonDeconvolutionDialog::getParameter() const
{
  if (isSineFunction())
    return ui->sineValueBox->value();
  return ui->singleValueBox->value();
}

void LucyRichardsonDeconvolutionDialog::browse()
{
  QSettings settings;
  QString rootpath = settings.value(path_setting,QDir::homePath()).toString();
  QString dir = QFileDialog::getExistingDirectory(this,QApplication::applicationDisplayName(),rootpath);
  ui->pathField->setText(dir);
  settings.setValue(path_setting,dir);
}


