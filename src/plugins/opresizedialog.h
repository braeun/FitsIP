/********************************************************************************
 *                                                                              *
 * FitsIP - resize image dialog                                                 *
 *                                                                              *
 * modified: 2023-02-04                                                         *
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

#ifndef OPRESIZEDIALOG_H
#define OPRESIZEDIALOG_H

#include <QDialog>

namespace Ui {
class OpResizeDialog;
}

class OpResizeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpResizeDialog(QWidget *parent = nullptr);
  ~OpResizeDialog();

  int getMode();

  int getFactor();

  bool isNearestNeighborInterpolation();

  bool isBilinearInterpolation();

private:
  Ui::OpResizeDialog *ui;
};

#endif // OPRESIZEDIALOG_H
