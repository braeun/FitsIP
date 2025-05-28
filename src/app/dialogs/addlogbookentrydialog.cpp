/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to add a logbook entry                                       *
 *                                                                              *
 * modified: 2025-02-20                                                         *
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
#include <fitsip/core/logbook/logbook.h>

AddLogbookEntryDialog::AddLogbookEntryDialog(const Logbook* logbook, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddLogbookEntryDialog)
{
  ui->setupUi(this);
  for (const QString& s : logbook->getProjects())
  {
    ui->projectBox->addItem(s);
  }
  ui->projectBox->setCurrentText(logbook->getProject());
  ui->imageBox->addItem("generic");
  for (auto& image : logbook->getImages())
  {
    ui->imageBox->addItem(image);
  }
}

AddLogbookEntryDialog::~AddLogbookEntryDialog()
{
  delete ui;
}

QString AddLogbookEntryDialog::getProject() const
{
  return ui->projectBox->currentText();
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

