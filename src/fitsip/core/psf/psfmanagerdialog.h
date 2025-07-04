/********************************************************************************
 *                                                                              *
 * FitsIP - PSF manager dialog                                                  *
 *                                                                              *
 * modified: 2025-06-06                                                         *
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

#ifndef PSFMANAGERDIALOG_H
#define PSFMANAGERDIALOG_H

#include <QDialog>

namespace Ui {
class PSFManagerDialog;
}

class ImageCollection;
class PreviewWidget;
class PSFWidget;

class PSFManagerDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PSFManagerDialog(ImageCollection* collection, QWidget *parent = nullptr);
  ~PSFManagerDialog();

  void updatePSFList();

private:
  void functionSelected(QString text);
  void preview();
  void addImagePSF();
  void deletePSF();
  void renamePSF();

  Ui::PSFManagerDialog *ui;
  PreviewWidget* previewWidget;
  PSFWidget* psfWidget;
  ImageCollection* collection;
};

#endif // PSFMANAGERDIALOG_H
