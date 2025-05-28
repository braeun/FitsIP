/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to stack images                                              *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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

#ifndef OPSTACKDIALOG_H
#define OPSTACKDIALOG_H

#include <QDialog>

namespace Ui {
class OpStackDialog;
}

class OpStackDialog : public QDialog
{
  Q_OBJECT
public:

  explicit OpStackDialog(QWidget *parent = nullptr);
  ~OpStackDialog();

  int getAlignment() const;

  bool isSubtractSky() const;

  bool isFullTemplateMatch() const;

  int getTemplateMatchRange() const;

  bool isAllowRotation() const;

  int getSearchBoxSize() const;

  int getStarBoxSize() const;

  int getStarMaxMovement() const;

private:
  Ui::OpStackDialog *ui;
};

#endif // OPSTACKDIALOG_H
