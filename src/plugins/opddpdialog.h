/********************************************************************************
 *                                                                              *
 * FitsIP - digital development processing dialog                               *
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

#ifndef OPDDPDIALOG_H
#define OPDDPDIALOG_H

#include <fitsbase/widgets/previewoptions.h>
#include <QDialog>

namespace Ui {
class OpDDPDialog;
}

class FitsImage;

class OpDDPDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpDDPDialog(QWidget *parent = nullptr);
  ~OpDDPDialog();

  void setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt);

  double getBackground() const;

  double getSigma() const;

  double getA() const;

  double getB() const;

private:
  void textFieldChanged();

  Ui::OpDDPDialog *ui;
};

#endif // OPDDPDIALOG_H
