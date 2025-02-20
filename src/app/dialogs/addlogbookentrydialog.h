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

#ifndef ADDLOGBOOKENTRYDIALOG_H
#define ADDLOGBOOKENTRYDIALOG_H

#include <fitsbase/logbook/logbookentry.h>
#include <QDialog>
#include <set>

class Logbook;

namespace Ui {
class AddLogbookEntryDialog;
}

class AddLogbookEntryDialog : public QDialog
{
  Q_OBJECT
public:
  explicit AddLogbookEntryDialog(const Logbook* logbook, QWidget *parent=nullptr);
  ~AddLogbookEntryDialog();

  QString getProject() const;

  LogbookEntry::Type getType() const;

  QString getImage() const;

  QString getText() const;

private:
  Ui::AddLogbookEntryDialog *ui;
};

#endif // ADDLOGBOOKENTRYDIALOG_H
