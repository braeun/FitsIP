/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create gaussian test image                                *
 *                                                                              *
 * modified: 2025-05-24                                                         *
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

#ifndef GAUSSIANTESTIMAGEDIALOG_H
#define GAUSSIANTESTIMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class GaussianTestImageDialog;
}

class GaussianTestImageDialog : public QDialog
{
  Q_OBJECT

public:
  explicit GaussianTestImageDialog(QWidget *parent = nullptr);
  ~GaussianTestImageDialog();

  int getWidth() const;

  int getHeight() const;

  double getAmplitude() const;

  double getCenterX() const;

  double getSigmaX() const;

  double getCenterY() const;

  double getSigmaY() const;

private:
  Ui::GaussianTestImageDialog *ui;
};

#endif // GAUSSIANTESTIMAGEDIALOG_H
