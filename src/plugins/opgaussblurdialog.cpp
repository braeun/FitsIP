/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian blur dialog                                                *
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

#include "opgaussblurdialog.h"
#include "ui_opgaussblurdialog.h"
#include "opgaussblur.h"
#include <fitsbase/fitsimage.h>
#include <fitsbase/histogram.h>
#include <fitsbase/settings.h>


OpGaussBlurDialog::OpGaussBlurDialog(QWidget *parent):AbstractPreviewDialog(parent),
  ui(new Ui::OpGaussBlurDialog)
{
  ui->setupUi(this);
  setPreviewLabel(ui->previewLabel);
  connect(ui->sigmaField,&QLineEdit::editingFinished,this,&OpGaussBlurDialog::textFieldChanged);
}

OpGaussBlurDialog::~OpGaussBlurDialog()
{
  delete ui;
}

double OpGaussBlurDialog::getSigma() const
{
  return ui->sigmaField->text().toDouble();
}



void OpGaussBlurDialog::textFieldChanged()
{
  updating = true;
  int32_t v = static_cast<int32_t>(ui->sigmaField->text().toDouble()*10);
  ui->sigmaSlider->setValue(v);
  updatePreview(ui->previewLabel);
  updating = false;
}

std::shared_ptr<FitsImage> OpGaussBlurDialog::getPreviewImage()
{
  auto img = std::make_shared<FitsImage>(*previewImage);
  OpGaussBlur op;
  op.blur(img,getSigma(),getSigma());
  return img;
}


void OpGaussBlurDialog::on_sigmaSlider_valueChanged(int value)
{
  if (!updating)
  {
    ui->sigmaField->setText(QString::number(value/10.0));
    updatePreview(ui->previewLabel);
  }
}

