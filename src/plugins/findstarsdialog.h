/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for star detection                                           *
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

#ifndef FINDSTARSDIALOG_H
#define FINDSTARSDIALOG_H

#include <QDialog>

namespace Ui {
class FindStarsDialog;
}

class FindStarsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FindStarsDialog(QWidget *parent = nullptr);
  ~FindStarsDialog();

  double getSkyMean() const;

  void setSkyMean(double v);

  double getSkySigma() const;

  void setSkySigma(double v);

  double getSkyMinSigma() const;

  void setSkyMinSigma(double v);

  double getFWHMMin() const;

  void setFWHMMin(double v);

  double getFWHMMax() const;

  void setFWHMMax(double v);

  double getRoundnessMin() const;

  void setRoundnessMin(double v);

  double getRoundnessMax() const;

  void setRoundnessMax(double v);

  double getSharpnessMin() const;

  void setSharpnessMin(double v);

  double getSharpnessMax() const;

  void setSharpnessMax(double v);

  double getHotnessMin() const;

  void setHotnessMin(double v);

  double getHotnessMax() const;

  void setHotnessMax(double v);

  int getTinyboxSize() const;

  void setTinyboxSize(int v);

  double getMoveMax() const;

  void setMoveMax(double v);

  bool isBlur() const;

  void setBlur(bool flag);

  int getIterations() const;

  void setIterations(int v);

private:
  Ui::FindStarsDialog *ui;
};

#endif // FINDSTARSDIALOG_H
