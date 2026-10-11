/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to edit image metadata                                       *
 *                                                                              *
 * modified: 2025-11-09                                                         *
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
#include <fitsip/core/imagemetadata.h>
#include <fitsip/core/db/database.h>

EditMetadataDialog::EditMetadataDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::EditMetadataDialog)
{
  ui->setupUi(this);
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  Database* db = Database::get();
  if (db)
  {
    ui->telescopeBox->addItems(db->getTelescopeList());
    ui->instrumentBox->addItems(db->getCameraList());
  }
}

EditMetadataDialog::~EditMetadataDialog()
{
  delete ui;
}

void EditMetadataDialog::displayMetadata(const ImageMetadata &data)
{
  ui->objectField->setText(data.getValue(ImageMetadata::keyObject));
  ui->observerField->setText(data.getValue(ImageMetadata::keyObserver));
  ui->telescopeBox->setCurrentText(data.getValue(ImageMetadata::keyTelescope));
  ui->instrumentBox->setCurrentText(data.getValue(ImageMetadata::keyInstrument));
  ui->exposureField->setText(data.getValue(ImageMetadata::keyExpTime));
  ui->dateField->setDateTime(data.getObsDateTime());
  ui->tableWidget->clearContents();
  ui->tableWidget->setRowCount(data.getEntries().size());
  int row = 0;
  for (const auto& entry : data.getEntries())
  {
    ui->tableWidget->setItem(row,0,new QTableWidgetItem(entry.first));
    ui->tableWidget->setItem(row,1,new QTableWidgetItem(entry.second.value));
    ui->tableWidget->setItem(row,2,new QTableWidgetItem(entry.second.comment));
    ++row;
  }
}

void EditMetadataDialog::commit(ImageMetadata &data)
{
  if (ui->tabWidget->currentIndex() == 0)
  {
    data.addEntry(ImageMetadata::keyObject,ui->objectField->text());
    data.addEntry(ImageMetadata::keyObserver,ui->observerField->text());
    data.addEntry(ImageMetadata::keyTelescope,ui->telescopeBox->currentText());
    data.addEntry(ImageMetadata::keyInstrument,ui->instrumentBox->currentText());
    data.addEntry(ImageMetadata::keyExpTime,ui->exposureField->text());
    data.setObsDateTime(ui->dateField->dateTime());
  }
  else
  {
    std::map<QString,ImageMetadata::Entry> entries;
    for (int r=0;r<ui->tableWidget->rowCount();++r)
    {
      QString key = ui->tableWidget->item(r,0)->text();
      QString value = ui->tableWidget->item(r,1)->text();
      QString comment = ui->tableWidget->item(r,2)->text();
      entries[key] = ImageMetadata::Entry{.value{value},.comment{comment}};
    }
    data.setEntries(entries);
  }
}
