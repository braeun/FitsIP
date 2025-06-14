/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create a test image fom a PSF                             *
 *                                                                              *
 * modified: 2025-06-01                                                         *
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
#include <fitsip/core/fitstypes.h>
#include <fitsip/core/widgets/psfwidget.h>
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

  void updatePSFList();

  QString getFunction() const;

  std::vector<ValueType> getParameters() const;

  int getWidth() const;

  int getHeight() const;

  double getAmplitude() const;

private:
  void functionSelected(QString text);

  Ui::PSFTestImageDialog *ui;
  PSFWidget *psfWidget;
};

#endif // PSFTESTIMAGEDIALOG_H
