/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian blur dialog                                                *
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

#ifndef OPGAUSSBLURDIALOG_H
#define OPGAUSSBLURDIALOG_H

#include <fitsbase/dialogs/abstractpreviewdialog.h>

namespace Ui {
class OpGaussBlurDialog;
}

class FitsImage;

class OpGaussBlurDialog : public AbstractPreviewDialog
{
  Q_OBJECT

public:
  explicit OpGaussBlurDialog(QWidget *parent = nullptr);
  ~OpGaussBlurDialog();

  double getSigma() const;

private slots:
  void on_sigmaSlider_valueChanged(int value);

private:
  void textFieldChanged();
  virtual std::shared_ptr<FitsImage> getPreviewImage();

  Ui::OpGaussBlurDialog *ui;
};

#endif // OPGAUSSBLURDIALOG_H
