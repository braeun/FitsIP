/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to edit image metadata                                       *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#include "editmetadatadialog.h"
#include "ui_editmetadatadialog.h"
#include <fitsbase/imagemetadata.h>

EditMetadataDialog::EditMetadataDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EditMetadataDialog)
{
  ui->setupUi(this);
}

EditMetadataDialog::~EditMetadataDialog()
{
  delete ui;
}

void EditMetadataDialog::displayMetadata(const ImageMetadata &data)
{
  ui->objectField->setText(data.object);
  ui->observerField->setText(data.observer);
  ui->telescopeField->setText(data.telescope);
  ui->instrumentField->setText(data.instrument);
  ui->exposureField->setText(QString::number(data.exposure));
  ui->dateField->setDateTime(data.date);
}

void EditMetadataDialog::commit(ImageMetadata &data)
{
  data.object = ui->objectField->text();
  data.observer = ui->observerField->text();
  data.telescope = ui->telescopeField->text();
  data.instrument = ui->instrumentField->text();
  data.exposure = ui->exposureField->text().toDouble();
  data.date = ui->dateField->dateTime();
}
