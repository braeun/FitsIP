/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to create box test image                                     *
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

#ifndef BOXTESTIMAGEDIALOG_H
#define BOXTESTIMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class BoxTestImageDialog;
}

class BoxTestImageDialog : public QDialog
{
  Q_OBJECT

public:
  explicit BoxTestImageDialog(QWidget *parent = nullptr);
  ~BoxTestImageDialog();

  int getWidth() const;

  int getHeight() const;

  double getAmplitude() const;

  int getCenterX() const;

  int getBoxWidth() const;

  int getCenterY() const;

  int getBoxHeight() const;

private:
  Ui::BoxTestImageDialog *ui;
};

#endif // BOXTESTIMAGEDIALOG_H
