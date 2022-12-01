/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for dialogs with a preview                       *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#ifndef ABSTRACTPREVIEWDIALOG_H
#define ABSTRACTPREVIEWDIALOG_H

#include <QDialog>

class FitsImage;
class QLabel;

class AbstractPreviewDialog: public QDialog
{
  Q_OBJECT
public:
  AbstractPreviewDialog(QWidget* parent=nullptr);
  virtual ~AbstractPreviewDialog();

  void setPreview(std::shared_ptr<FitsImage> image, QRect selection);

protected:
  virtual std::shared_ptr<FitsImage> getPreviewImage() = 0;
  void updatePreview(QLabel* label);
  void setPreviewLabel(QLabel* label);

  bool updating;
  std::shared_ptr<FitsImage> previewImage;
  QLabel* previewLabel;
};

#endif // ABSTRACTPREVIEWDIALOG_H
