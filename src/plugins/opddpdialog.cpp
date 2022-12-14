/********************************************************************************
 *                                                                              *
 * FitsIP - digital development processing dialog                               *
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

#include "opddpdialog.h"
#include "ui_opddpdialog.h"
#include "opddp.h"
#include <fitsimage.h>
#include <histogram.h>
#include <settings.h>

OpDDPDialog::OpDDPDialog(QWidget *parent):AbstractPreviewDialog(parent),
  ui(new Ui::OpDDPDialog)
{
  ui->setupUi(this);
  setPreviewLabel(ui->previewLabel);
  connect(ui->sigmaField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
  connect(ui->backgroundField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
  connect(ui->aField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
  connect(ui->bField,&QLineEdit::editingFinished,this,&OpDDPDialog::textFieldChanged);
}

OpDDPDialog::~OpDDPDialog()
{
  delete ui;
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
  updatePreview(ui->previewLabel);
}

std::shared_ptr<FitsImage> OpDDPDialog::getPreviewImage()
{
  auto img = std::make_shared<FitsImage>(*previewImage);
  OpDDP op;
  op.ddp(img,getSigma(),getBackground(),getA(),getB());
  return img;
}



