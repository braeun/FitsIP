/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create a test image fom a PSF                             *
 *                                                                              *
 * modified: 2023-02-03                                                         *
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

#ifndef PSFTESTIMAGEDIALOG_H
#define PSFTESTIMAGEDIALOG_H

#include <QDialog>
#include <fitstypes.h>
#include <vector>

namespace Ui {
class PSFTestImageDialog;
}

class PSFTestImageDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PSFTestImageDialog(QWidget *parent = nullptr);
  ~PSFTestImageDialog();

  QString getFunction() const;

  std::vector<ValueType> getParameters() const;

  int getWidth() const;

  int getHeight() const;

  double getAmplitude() const;

  double getCenterX() const;

  double getCenterY() const;

private:
  Ui::PSFTestImageDialog *ui;
};

#endif // PSFTESTIMAGEDIALOG_H
