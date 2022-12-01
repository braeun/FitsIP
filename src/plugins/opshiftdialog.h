/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to shift image with subpixel accuracy                        *
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

#ifndef OPSHIFTDIALOG_H
#define OPSHIFTDIALOG_H

#include <QDialog>

namespace Ui {
class OpShiftDialog;
}

class OpShiftDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpShiftDialog(QWidget *parent = nullptr);
  ~OpShiftDialog();

  void setImageSize(int32_t w, int32_t h);

  double getDeltaX() const;

  double getDeltaY() const;

private:
  Ui::OpShiftDialog *ui;
  int32_t imgWidth;
  int32_t imgHeight;
};

#endif // OPSHIFTDIALOG_H
