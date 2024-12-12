/********************************************************************************
 *                                                                              *
 * FitsIP - median filter dialog                                                *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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
#include <fitsbase/fitsimage.h>
#include <fitsbase/histogram.h>
#include <fitsbase/settings.h>


OpMedianDialog::OpMedianDialog(QWidget *parent) :
  AbstractPreviewDialog(parent),
  ui(new Ui::OpMedianDialog)
{
  ui->setupUi(this);
  setPreviewLabel(ui->previewLabel);
  connect(ui->thresholdField,&QLineEdit::editingFinished,this,&OpMedianDialog::textFieldChanged);
  connect(ui->kernelSizeBox,&QSpinBox::textChanged,this,&OpMedianDialog::textFieldChanged);
}

OpMedianDialog::~OpMedianDialog()
{
  delete ui;
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
  updating = true;
  updatePreview(ui->previewLabel);
  updating = false;
}

std::shared_ptr<FitsImage> OpMedianDialog::getPreviewImage()
{
  auto img = std::make_shared<FitsImage>(*previewImage);
  OpMedian op;
  op.filter(img,getThreshold(),getSize());
  return img;
}

