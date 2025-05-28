/********************************************************************************
 *                                                                              *
 * FitsIP - kernel filter dialog                                                *
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

#ifndef OPKERNELDIALOG_H
#define OPKERNELDIALOG_H

#include <fitsip/core/widgets/previewoptions.h>
#include <QDialog>

class FitsImage;
class PreviewWidget;

namespace Ui {
class OpKernelDialog;
}

class OpKernelDialog: public QDialog
{
  Q_OBJECT
public:
  explicit OpKernelDialog(QWidget *parent = nullptr);
  ~OpKernelDialog();

  void setSourceImage(std::shared_ptr<FitsImage> img, QRect selection, const PreviewOptions& opt);

  void setKernelNames(const std::vector<QString>& list);

  QString getKernelName() const;

private slots:
  void on_kernelBox_currentTextChanged(const QString &arg1);

private:
  Ui::OpKernelDialog *ui;
  PreviewWidget *previewWidget;
};

#endif // OPKERNELDIALOG_H
