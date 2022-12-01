/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to set the log book filter                                   *
 *                                                                              *
 * modified: 2022-11-30                                                         *
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

#ifndef LOGBOOKFILTER1DIALOG_H
#define LOGBOOKFILTER1DIALOG_H

#include <QDialog>

class Logbook;
class LogbookFilter;

namespace Ui {
class LogbookFilter1Dialog;
}

class LogbookFilter1Dialog : public QDialog
{
  Q_OBJECT

public:
  explicit LogbookFilter1Dialog(const Logbook* logbook, QWidget *parent = nullptr);
  ~LogbookFilter1Dialog();

  LogbookFilter getFilter() const;

private slots:
  void on_projectBox_currentTextChanged(const QString &text);

private:
  Ui::LogbookFilter1Dialog *ui;
  const Logbook* logbook;
};

#endif // LOGBOOKFILTER1DIALOG_H
