/********************************************************************************
 *                                                                              *
 * FitsIP - median filter dialog                                                *
 *                                                                              *
 * modified: 2024-12-16                                                         *
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

#ifndef OPMEDIANDIALOG_H
#define OPMEDIANDIALOG_H

#include <fitsbase/widgets/previewoptions.h>
#include <QDialog>

namespace Ui {
class OpMedianDialog;
}

class FitsImage;

class OpMedianDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpMedianDialog(QWidget *parent = nullptr);
  ~OpMedianDialog();

  void setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt);

  int getSize() const;

  double getThreshold() const;

private:
  void textFieldChanged();

  Ui::OpMedianDialog *ui;
};

#endif // OPMEDIANDIALOG_H
