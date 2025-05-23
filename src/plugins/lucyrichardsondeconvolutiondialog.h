/********************************************************************************
 *                                                                              *
 * FitsIP - Lucy Richardson deconvolution dialog                                *
 *                                                                              *
 * modified: 2025-05-23                                                         *
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

#ifndef LUCYRICHARDSONDECONVOLUTIONDIALOG_H
#define LUCYRICHARDSONDECONVOLUTIONDIALOG_H

#include <fitsbase/fitstypes.h>
#include <QDialog>

namespace Ui {
class LucyRichardsonDeconvolutionDialog;
}

class LucyRichardsonDeconvolutionDialog : public QDialog
{
  Q_OBJECT
public:
  explicit LucyRichardsonDeconvolutionDialog(QWidget *parent = nullptr);
  ~LucyRichardsonDeconvolutionDialog();

  void updatePSFList();

  QString getFunction() const;

  std::vector<ValueType> getParameters() const;

  int getIterationCount() const;

  bool isCutImage() const;

  bool isSineFunction() const;

  bool isConstMultFunction() const;

  ValueType getParameter() const;

private:
  Ui::LucyRichardsonDeconvolutionDialog *ui;
};

#endif // LUCYRICHARDSONDECONVOLUTIONDIALOG_H
