/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian blur dialog                                                *
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

#include "opgaussblurdialog.h"
#include "ui_opgaussblurdialog.h"
#include "opgaussblur.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/settings.h>
#include <fitsip/core/widgets/previewwidget.h>


OpGaussBlurDialog::OpGaussBlurDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpGaussBlurDialog),
  updating(false)
{
  ui->setupUi(this);
  previewWidget = new PreviewWidget(ui->groupBox_2);
  previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
  ui->verticalLayout_2->addWidget(previewWidget);
  connect(ui->sigmaField,&QLineEdit::editingFinished,this,&OpGaussBlurDialog::textFieldChanged);
}

OpGaussBlurDialog::~OpGaussBlurDialog()
{
  delete ui;
}

void OpGaussBlurDialog::setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt)
{
  previewWidget->setOptions(opt);
  previewWidget->setSourceImage(img,selection);
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
  updatePreview();
  updating = false;
}

void OpGaussBlurDialog::updatePreview()
{
  if (previewWidget->getSourceImage())
  {
    auto img = std::make_shared<FitsImage>(*previewWidget->getSourceImage());
    OpGaussBlur op;
    op.blur(img,getSigma(),getSigma());
    previewWidget->updatePreview(img);
  }
}


void OpGaussBlurDialog::on_sigmaSlider_valueChanged(int value)
{
  if (!updating)
  {
    ui->sigmaField->setText(QString::number(value/10.0));
    updatePreview();
  }
}

