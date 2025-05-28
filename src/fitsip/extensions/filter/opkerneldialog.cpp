/********************************************************************************
 *                                                                              *
 * FitsIP - kernel filter dialog                                                *
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

#include "opkerneldialog.h"
#include "ui_opkerneldialog.h"
#include "opkernel.h"
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/kernelrepository.h>
#include <fitsip/core/widgets/previewwidget.h>

OpKernelDialog::OpKernelDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::OpKernelDialog)
{
  ui->setupUi(this);
  previewWidget = new PreviewWidget(ui->groupBox_2);
  previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
  ui->verticalLayout_2->addWidget(previewWidget);
}

OpKernelDialog::~OpKernelDialog()
{
  delete ui;
}

void OpKernelDialog::setKernelNames(const std::vector<QString> &list)
{
  QString old = ui->kernelBox->currentText();
  bool inList = false;
  ui->kernelBox->clear();
  if (!list.empty())
  {
    for (const QString& n : list)
    {
      ui->kernelBox->addItem(n);
      if (n == old) inList = true;
    }
    ui->kernelBox->setCurrentText(inList?old:list.front());
  }
}

void OpKernelDialog::setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt)
{
  previewWidget->setOptions(opt);
  previewWidget->setSourceImage(img,selection);
}

QString OpKernelDialog::getKernelName() const
{
  return ui->kernelBox->currentText();
}



void OpKernelDialog::on_kernelBox_currentTextChanged(const QString&)
{
  if (previewWidget->getSourceImage())
  {
    Kernel kernel = KernelRepository::instance().getKernel(getKernelName());
    if (kernel.isEmpty()) return ;
    auto img = std::make_shared<FitsImage>(*previewWidget->getSourceImage());
    OpKernel op;
    op.convolve(img,kernel);
    previewWidget->updatePreview(img);
  }
}
