/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to analyse a profile                                         *
 *                                                                              *
 * modified: 2025-01-12                                                         *
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

#ifndef ANALYSEPROFILEDIALOG_H
#define ANALYSEPROFILEDIALOG_H

#include <QDialog>

class FitsObject;

namespace Ui {
class AnalyseProfileDialog;
}

class AnalyseProfileDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AnalyseProfileDialog(QWidget *parent = nullptr);
  ~AnalyseProfileDialog();

  void setImage(FitsObject* obj);

  virtual void done(int r);

private:
  Ui::AnalyseProfileDialog *ui;
  FitsObject* object;
};

#endif // ANALYSEPROFILEDIALOG_H
