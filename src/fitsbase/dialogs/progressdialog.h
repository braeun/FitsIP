/********************************************************************************
 *                                                                              *
 * FitsIP - dialog showing the progress of an operation                         *
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

#ifndef PROGESSDIALOG_H
#define PROGESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ProgressDialog(QWidget *parent = nullptr);
  ~ProgressDialog();

  void setTitle(const QString& title);

  void appendMessage(const QString& txt);

  void setMaximum(int max);

  void setProgress(int value);

  bool isCancelled() const;

public slots:

  virtual void reject() override;

private:
  Ui::ProgressDialog *ui;
  bool cancelled;
};

#endif // PROGESSDIALOG_H
