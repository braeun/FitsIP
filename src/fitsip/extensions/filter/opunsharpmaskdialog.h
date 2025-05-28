/********************************************************************************
 *                                                                              *
 * FitsIP - unsharp masking dialog                                              *
 *                                                                              *
 * modified: 2024-12-16                                                         *
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

#ifndef OPUNSHARPMASKDIALOG_H
#define OPUNSHARPMASKDIALOG_H

#include <fitsip/core/widgets/previewoptions.h>
#include <QDialog>

namespace Ui {
class OpUnsharpMaskDialog;
}

class FitsImage;
class PreviewWidget;

class OpUnsharpMaskDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpUnsharpMaskDialog(QWidget *parent = nullptr);
  ~OpUnsharpMaskDialog();

  void setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt);

  double getSigma() const;

  double getStrength() const;

private slots:
  void on_sigmaSlider_valueChanged(int value);

  void on_strengthSlider_valueChanged(int value);

private:
  void textFieldChanged();
  void updatePreview();

  Ui::OpUnsharpMaskDialog *ui;
  PreviewWidget *previewWidget;
  bool updating;
};

#endif // OPUNSHARPMASKDIALOG_H
