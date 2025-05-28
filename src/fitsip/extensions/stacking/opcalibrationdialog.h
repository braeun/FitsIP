/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for image calibration with flatfield and dark image          *
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

#ifndef OPCALIBRATIONDIALOG_H
#define OPCALIBRATIONDIALOG_H

#include <QDialog>
#include <memory>

class ImageCollection;
class ImageSelectWidget;
class FitsObject;

namespace Ui {
class OpCalibrationDialog;
}

class OpCalibrationDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpCalibrationDialog(QWidget *parent = nullptr);
  ~OpCalibrationDialog();

  void setImageCollection(ImageCollection* collection);

  std::shared_ptr<FitsObject> getDarkFrame();

  std::shared_ptr<FitsObject> getFlatField();

  void setOutputPath(const QString& path);

  QString getOutputPath() const;

  QString getPrefix() const;

  QString getSuffix() const;

private:
  void browse();

  Ui::OpCalibrationDialog *ui;
  ImageSelectWidget *darkFrameSelectWidget;
  ImageSelectWidget *flatFieldSelectWidget;
};

#endif // OPCALIBRATIONDIALOG_H
