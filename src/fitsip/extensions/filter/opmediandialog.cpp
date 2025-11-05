/********************************************************************************
 *                                                                              *
 * FitsIP - median filter dialog                                                *
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

#include "opmediandialog.h"
#include "ui_opmediandialog.h"
#include "opmedian.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/settings.h>
#include <fitsip/core/widgets/previewwidget.h>


OpMedianDialog::OpMedianDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpMedianDialog)
{
  ui->setupUi(this);
  previewWidget = new PreviewWidget(ui->groupBox_2);
  previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
  ui->horizontalLayout_2->addWidget(previewWidget);
  connect(ui->thresholdField,&QLineEdit::editingFinished,this,&OpMedianDialog::textFieldChanged);
  connect(ui->kernelSizeBox,&QSpinBox::textChanged,this,&OpMedianDialog::textFieldChanged);
}

OpMedianDialog::~OpMedianDialog()
{
  delete ui;
}

void OpMedianDialog::setSourceImage(const FitsImage& img, QRect selection, const PreviewOptions& opt)
{
  previewWidget->setOptions(opt);
  previewWidget->setSourceImage(img,selection);
}

int OpMedianDialog::getSize() const
{
  return ui->kernelSizeBox->value();
}

double OpMedianDialog::getThreshold() const
{
  return ui->thresholdField->text().toDouble();
}



void OpMedianDialog::textFieldChanged()
{
  if (previewWidget->getSourceImage())
  {
    FitsImage img(previewWidget->getSourceImage());
    OpMedian op;
    op.filter(&img,getThreshold(),getSize());
    previewWidget->updatePreview(img);
  }
}


