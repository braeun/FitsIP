/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to add a logbook entry                                       *
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

#include "addlogbookentrydialog.h"
#include "ui_addlogbookentrydialog.h"
#include <imagecollection.h>

AddLogbookEntryDialog::AddLogbookEntryDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddLogbookEntryDialog)
{
  ui->setupUi(this);
  for (auto image : ImageCollection::getGlobal().getFiles())
  {
    ui->imageBox->addItem(image->getImage()->getName());
  }
}

AddLogbookEntryDialog::~AddLogbookEntryDialog()
{
  delete ui;
}

LogbookEntry::Type AddLogbookEntryDialog::getType() const
{
  switch (ui->typeBox->currentIndex())
  {
    case 0:
    default:
      return LogbookEntry::Note;
      break;
  }
}

QString AddLogbookEntryDialog::getImage() const
{
  return ui->imageBox->currentText();
}

QString AddLogbookEntryDialog::getText() const
{
  return ui->textField->toPlainText();
}

