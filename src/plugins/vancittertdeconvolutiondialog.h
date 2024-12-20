/********************************************************************************
 *                                                                              *
 * FitsIP - vanCittert deconvolution dialog                                     *
 *                                                                              *
 * modified: 2022-11-25                                                         *
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

#ifndef VANCITTERTDECONVOLUTIONDIALOG_H
#define VANCITTERTDECONVOLUTIONDIALOG_H

#include <fitsbase/fitstypes.h>
#include <QDialog>

namespace Ui {
class VanCittertDeconvolutionDialog;
}

class VanCittertDeconvolutionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VanCittertDeconvolutionDialog(QWidget *parent = nullptr);
  ~VanCittertDeconvolutionDialog();

  QString getFunction() const;

  std::vector<ValueType> getParameters() const;

  int getIterationCount() const;

  bool isCutImage() const;

  bool isSineFunction() const;

  bool isConstMultFunction() const;

  ValueType getParameter() const;

private:
  Ui::VanCittertDeconvolutionDialog *ui;
};

#endif // VANCITTERTDECONVOLUTIONDIALOG_H
