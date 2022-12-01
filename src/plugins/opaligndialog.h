/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to align images                                   *
 *                                                                              *
 * modified: 2022-11-22                                                         *
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

#ifndef OPALIGNDIALOG_H
#define OPALIGNDIALOG_H

#include <QDialog>

namespace Ui {
class OpAlignDialog;
}

class OpAlignDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpAlignDialog(QWidget *parent = nullptr);
  ~OpAlignDialog();

  void setOutputPath(const QString& p);

  QString getOutputPath() const;

private slots:
  void on_browseButton_clicked();

private:
  Ui::OpAlignDialog *ui;
  QString outputPath;
};

#endif // OPALIGNDIALOG_H
