/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to crop image                                                *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#ifndef OPCROPDIALOG_H
#define OPCROPDIALOG_H

#include <fitsbase/fitstypes.h>
#include <QDialog>

namespace Ui {
class OpCropDialog;
}

class OpCropDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpCropDialog(QWidget *parent = nullptr);
  ~OpCropDialog();

  bool isManual() const;

  void setSelection(QRect r);

  QRect getSelection() const;

  ValueType getThreshold() const;

  int getBorder() const;

private:
  Ui::OpCropDialog *ui;
};

#endif // OPCROPDIALOG_H
