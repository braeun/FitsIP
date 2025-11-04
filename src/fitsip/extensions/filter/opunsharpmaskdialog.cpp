/********************************************************************************
 *                                                                              *
 * FitsIP - unsharp masking dialog                                              *
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

#include "opunsharpmaskdialog.h"
#include "ui_opunsharpmaskdialog.h"
#include "opunsharpmask.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/settings.h>
#include <fitsip/core/widgets/previewwidget.h>

OpUnsharpMaskDialog::OpUnsharpMaskDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpUnsharpMaskDialog),
  updating(false)
{
  ui->setupUi(this);
  previewWidget = new PreviewWidget(ui->groupBox_2);
  previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(1);
  sizePolicy.setVerticalStretch(1);
  sizePolicy.setHeightForWidth(previewWidget->sizePolicy().hasHeightForWidth());
  previewWidget->setSizePolicy(sizePolicy);
  ui->verticalLayout_2->addWidget(previewWidget);

  connect(ui->sigmaField,&QLineEdit::editingFinished,this,&OpUnsharpMaskDialog::textFieldChanged);
  connect(ui->strengthField,&QLineEdit::editingFinished,this,&OpUnsharpMaskDialog::textFieldChanged);
}

OpUnsharpMaskDialog::~OpUnsharpMaskDialog()
{
  delete ui;
}

void OpUnsharpMaskDialog::setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt)
{
  previewWidget->setOptions(opt);
  previewWidget->setSourceImage(img,selection);
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
  updatePreview();
  updating = false;
}

void OpUnsharpMaskDialog::updatePreview()
{
  if (previewWidget->getSourceImage())
  {
    auto img = std::make_shared<FitsImage>(*previewWidget->getSourceImage());
    OpUnsharpMask op;
    op.unsharpmask(img.get(),getSigma(),getStrength());
    previewWidget->updatePreview(img);
  }
}


void OpUnsharpMaskDialog::on_sigmaSlider_valueChanged(int value)
{
  if (!updating)
  {
    ui->sigmaField->setText(QString::number(value/10.0));
    updatePreview();
  }
}

void OpUnsharpMaskDialog::on_strengthSlider_valueChanged(int value)
{
  if (!updating)
  {
    ui->strengthField->setText(QString::number(value/10.0));
    updatePreview();
  }
}
