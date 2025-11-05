/********************************************************************************
 *                                                                              *
 * FitsIP - gaussian blur dialog                                                *
 *                                                                              *
 * modified: 2025-11-04                                                         *
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

#include <fitsip/core/widgets/previewoptions.h>
#include <QDialog>

class FitsImage;

namespace Ui {
class OpGaussBlurDialog;
}

class FitsImage;
class PreviewWidget;

class OpGaussBlurDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpGaussBlurDialog(QWidget *parent = nullptr);
  ~OpGaussBlurDialog();

  void setSourceImage(const FitsImage& img, QRect selection, const PreviewOptions& opt);

  double getSigma() const;

private:
  void sigmaSliderChanged(int value);
  void textFieldChanged();
  void updatePreview();

  Ui::OpGaussBlurDialog *ui;
  PreviewWidget *previewWidget;
  bool updating;

  static double sliderScale;
};

#endif // OPGAUSSBLURDIALOG_H
