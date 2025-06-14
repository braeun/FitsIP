/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create a test image fom a PSF                             *
 *                                                                              *
 * modified: 2025-06-01                                                         *
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

#include "psftestimagedialog.h"
#include "ui_psftestimagedialog.h"
#include <fitsip/core/psf/psffactory.h>

PSFTestImageDialog::PSFTestImageDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PSFTestImageDialog)
{
  ui->setupUi(this);
  psfWidget = new PSFWidget(ui->groupBox);
  psfWidget->setObjectName(QString::fromUtf8("psfWidget"));
  ui->verticalLayout_2->insertWidget(0,psfWidget);
}

PSFTestImageDialog::~PSFTestImageDialog()
{
  delete ui;
}

void PSFTestImageDialog::updatePSFList()
{
  psfWidget->updatePSFList();
}

QString PSFTestImageDialog::getFunction() const
{
  return psfWidget->getFunction();
}

std::vector<ValueType> PSFTestImageDialog::getParameters() const
{
  return psfWidget->getParameters();
}

int PSFTestImageDialog::getWidth() const
{
  return ui->widthBox->value();
}

int PSFTestImageDialog::getHeight() const
{
  return  ui->heightBox->value();
}

double PSFTestImageDialog::getAmplitude() const
{
  return ui->amplitudeField->text().toDouble();
}

void PSFTestImageDialog::functionSelected(QString name)
{
  const PSF* psf = PSFFactory::getInstance()->getPSF(name);
  if (psf->isFixedSize())
  {
  }
  ui->widthBox->setEnabled(!psf->isFixedSize());
  ui->heightBox->setEnabled(!psf->isFixedSize());
}


