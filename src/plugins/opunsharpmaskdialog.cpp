/********************************************************************************
 *                                                                              *
 * FitsIP - unsharp masking dialog                                              *
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

#include "opunsharpmaskdialog.h"
#include "ui_opunsharpmaskdialog.h"
#include "opunsharpmask.h"
#include <fitsimage.h>
#include <histogram.h>
#include <settings.h>

OpUnsharpMaskDialog::OpUnsharpMaskDialog(QWidget *parent):AbstractPreviewDialog(parent),
  ui(new Ui::OpUnsharpMaskDialog)
{
  ui->setupUi(this);
  setPreviewLabel(ui->previewLabel);
  connect(ui->sigmaField,&QLineEdit::editingFinished,this,&OpUnsharpMaskDialog::textFieldChanged);
  connect(ui->strengthField,&QLineEdit::editingFinished,this,&OpUnsharpMaskDialog::textFieldChanged);
}

OpUnsharpMaskDialog::~OpUnsharpMaskDialog()
{
  delete ui;
}

double OpUnsharpMaskDialog::getSigma() const
{
  return ui->sigmaField->text().toDouble();
}

double OpUnsharpMaskDialog::getStrength() const
{
  return ui->strengthField->text().toDouble();
}



void OpUnsharpMaskDialog::textFieldChanged()
{
  updating = true;
  int32_t v = static_cast<int32_t>(ui->sigmaField->text().toDouble()*10);
  ui->sigmaSlider->setValue(v);
  v = static_cast<int32_t>(ui->strengthField->text().toDouble()*10);
  ui->strengthSlider->setValue(v);
  updatePreview(ui->previewLabel);
  updating = false;
}

std::shared_ptr<FitsImage> OpUnsharpMaskDialog::getPreviewImage()
{
  auto img = std::make_shared<FitsImage>(*previewImage);
  OpUnsharpMask op;
  op.unsharpmask(img,getSigma(),getStrength());
  return img;
}


void OpUnsharpMaskDialog::on_sigmaSlider_valueChanged(int value)
{
  if (!updating)
  {
    ui->sigmaField->setText(QString::number(value/10.0));
    updatePreview(ui->previewLabel);
  }
}

void OpUnsharpMaskDialog::on_strengthSlider_valueChanged(int value)
{
  if (!updating)
  {
    ui->strengthField->setText(QString::number(value/10.0));
    updatePreview(ui->previewLabel);
  }
}
