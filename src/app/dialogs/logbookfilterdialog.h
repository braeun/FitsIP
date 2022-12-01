/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to set the log book filter                                   *
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

#ifndef LOGBOOKFILTERDIALOG_H
#define LOGBOOKFILTERDIALOG_H

#include <QDialog>

class Logbook;
class LogbookFilter;

namespace Ui {
class LogbookFilterDialog;
}

class LogbookFilterDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LogbookFilterDialog(const Logbook* logbook, const LogbookFilter& filter, QWidget *parent = nullptr);
  ~LogbookFilterDialog();

  LogbookFilter getFilter() const;

public slots:

  void on_addImageButton_clicked();

  void on_removeImageButton_clicked();

private:
  Ui::LogbookFilterDialog *ui;
};

#endif // LOGBOOKFILTERDIALOG_H
