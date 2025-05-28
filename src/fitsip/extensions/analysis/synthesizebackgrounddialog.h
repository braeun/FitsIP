/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to synthesize background from image                          *
 *                                                                              *
 * modified: 2025-02-09                                                         *
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

#ifndef SYNTHESIZEBACKGROUNDDIALOG_H
#define SYNTHESIZEBACKGROUNDDIALOG_H

#include <fitsip/core/fitstypes.h>
#include <QDialog>

namespace Ui {
class SynthesizeBackgroundDialog;
}

class SynthesizeBackgroundDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SynthesizeBackgroundDialog(QWidget *parent = nullptr);
  ~SynthesizeBackgroundDialog();

  void setSky(AverageResult avg);

  int getSelectionMode() const;

  int getPointsCount() const;

  int getPolynomDegree() const;

  double getBackground() const;

private:
  Ui::SynthesizeBackgroundDialog *ui;
};

#endif // SYNTHESIZEBACKGROUNDDIALOG_H
