/********************************************************************************
 *                                                                              *
 * FitsIP - measure the distance dialog                                         *
 *                                                                              *
 * modified: 2025-03-13                                                         *
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

#ifndef MEASUREDISTANCEDIALOG_H
#define MEASUREDISTANCEDIALOG_H

#include <QDialog>
#include <fitsbase/pixel.h>
#include <fitsbase/io/db.h>

class PixelList;

namespace Ui {
class MeasureDistanceDialog;
}

class MeasureDistanceDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MeasureDistanceDialog(QWidget *parent = nullptr);
  ~MeasureDistanceDialog();

  void setPixelList(PixelList* list);

protected:
  void showEvent(QShowEvent *event) override;

private:
  void cameraSelected(const QString& name);
  void calculate();
  void save();

  Ui::MeasureDistanceDialog *ui;
  double p1x;
  double p1y;
  double p2x;
  double p2y;
  std::vector<db::Camera> cameras;
};

#endif // MEASUREDISTANCEDIALOG_H
