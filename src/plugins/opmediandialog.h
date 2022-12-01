/********************************************************************************
 *                                                                              *
 * FitsIP - median filter dialog                                                *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#ifndef OPMEDIANDIALOG_H
#define OPMEDIANDIALOG_H

#include <dialogs/abstractpreviewdialog.h>

namespace Ui {
class OpMedianDialog;
}

class FitsImage;

class OpMedianDialog : public AbstractPreviewDialog
{
  Q_OBJECT

public:
  explicit OpMedianDialog(QWidget *parent = nullptr);
  ~OpMedianDialog();

  int getSize() const;

  double getThreshold() const;

private:
  void textFieldChanged();
  virtual std::shared_ptr<FitsImage> getPreviewImage();

  Ui::OpMedianDialog *ui;
};

#endif // OPMEDIANDIALOG_H
