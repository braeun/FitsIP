/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create a test image with a Moffat distribution            *
 *                                                                              *
 * modified: 2025-11-01                                                         *
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

#ifndef MOFFATTESTIMAGEDIALOG_H
#define MOFFATTESTIMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class MoffatTestImageDialog;
}

class MoffatTestImageDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MoffatTestImageDialog(QWidget *parent = nullptr);
  ~MoffatTestImageDialog();

  int getWidth() const;

  int getHeight() const;

  double getCenterX() const;

  double getCenterY() const;

  double getAlphaX() const;

  double getAlphaY() const;

  double getBeta() const;

private:
  Ui::MoffatTestImageDialog *ui;
};

#endif // MOFFATTESTIMAGEDIALOG_H
