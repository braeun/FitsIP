/********************************************************************************
 *                                                                              *
 * FitsIP - digital development processing dialog                               *
 *                                                                              *
 * modified: 2025-02-09                                                         *
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

#include "opddpdialog.h"
#include "ui_opddpdialog.h"
#include "opddp.h"
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/settings.h>
#include <fitsip/core/widgets/previewwidget.h>

OpDDPDialog::OpDDPDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpDDPDialog)
{
  ui->setupUi(this);
  previewWidget = new PreviewWidget(ui->groupBox_2);
  previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
  ui->verticalLayout_2->addWidget(previewWidget);
  connect(ui->sigmaField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
  connect(ui->backgroundField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
  connect(ui->aField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
  connect(ui->bField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
}

OpDDPDialog::~OpDDPDialog()
{
  delete ui;
}

void OpDDPDialog::setSourceImage(const FitsImage& img, QRect selection, const PreviewOptions& opt)
{
  previewWidget->setOptions(opt);
  previewWidget->setSourceImage(img,selection);
}

double OpDDPDialog::getBackground() const
{
  return ui->backgroundField->text().toDouble();
}

double OpDDPDialog::getSigma() const
{
  return ui->sigmaField->text().toDouble();
}

double OpDDPDialog::getA() const
{
  return ui->aField->text().toDouble();
}

double OpDDPDialog::getB() const
{
  return ui->bField->text().toDouble();
}


void OpDDPDialog::textFieldChanged()
{
  if (previewWidget->getSourceImage())
  {
    FitsImage img(previewWidget->getSourceImage());
    OpDDP op;
    op.ddp(&img,getSigma(),getBackground(),getA(),getB());
    previewWidget->updatePreview(img);
  }
}

