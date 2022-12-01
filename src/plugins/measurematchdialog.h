/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to match two images                               *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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

#ifndef MEASUREMATCHDIALOG_H
#define MEASUREMATCHDIALOG_H

#include <QDialog>
#include <memory>

namespace Ui {
class MeasureMatchDialog;
}

class ImageCollection;
class FileObject;

class MeasureMatchDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MeasureMatchDialog(QWidget *parent = nullptr);
  ~MeasureMatchDialog();

  void setImageCollection(ImageCollection* collection);

  std::shared_ptr<FileObject> getImage();

  bool isMatchFullImage() const;

  int32_t getMatchRange() const;

  int32_t getScaleFactor() const;

  int32_t getFirstPassDelta() const;

  int32_t getSubsample() const;

private:
  Ui::MeasureMatchDialog *ui;
};

#endif // MEASUREMATCHDIALOG_H
