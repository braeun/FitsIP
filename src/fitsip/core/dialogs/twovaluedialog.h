/********************************************************************************
 *                                                                              *
 * FitsIP - simple dialog to enter two values                                   *
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

#ifndef TWOVALUEDIALOG_H
#define TWOVALUEDIALOG_H

#include <QDialog>

namespace Ui {
class TwoValueDialog;
}

class TwoValueDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TwoValueDialog(QWidget *parent = nullptr);
  ~TwoValueDialog();

  void setTitle(QString title);

  void setValue1Label(QString label, QString unit="");

  void setValue2Label(QString label, QString unit="");

  QString getValue1() const;

  void setValue1(QString v);

  QString getValue2() const;

  void setValue2(QString v);

  static std::pair<QString,QString> query(QWidget *parent, QString title, QString label1, QString label2, QString unit1="", QString unit2="", bool* ok=nullptr);

  static std::pair<int,int> queryInt(QWidget *parent, QString title, QString label1, QString label2, QString unit1="", QString unit2="", bool* ok=nullptr);

private:
  Ui::TwoValueDialog *ui;
};

#endif // TWOVALUEDIALOG_H
