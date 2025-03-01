/********************************************************************************
 *                                                                              *
 * FitsIP - configuration dialog                                                *
 *                                                                              *
 * modified: 2025-02-28                                                         *
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

#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>


class AppSettings;

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConfigurationDialog(QWidget *parent = nullptr);
  ~ConfigurationDialog();

  void commit();

private:
  void commitFields(AppSettings& settings);
  void updateFields(const AppSettings& settings);
  void browse(QLineEdit* field, QString path);

  Ui::ConfigurationDialog *ui;
};

#endif // CONFIGURATIONDIALOG_H
