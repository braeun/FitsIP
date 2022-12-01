/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the profiles and associated controls              *
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

#ifndef PROFILEVIEW_H
#define PROFILEVIEW_H

#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <QWidget>
#include <memory>

class FileObject;
class QwtPlotGrid;
class QwtPlotPicker;
class QwtPlotZoomer;

namespace Ui {
class ProfileView;
}

class ProfileView : public QWidget
{
  Q_OBJECT

public:
  explicit ProfileView(QWidget *parent = nullptr);
  ~ProfileView();

  void setImage(std::shared_ptr<FileObject> obj);

public slots:
  void updateCursor(QPoint p);

  void setCursor(QPoint p);

protected:
  void changeEvent(QEvent* event);

private slots:
  void on_logYBox_toggled(bool checked);

private:
  void redraw();

  Ui::ProfileView *ui;
  std::shared_ptr<FileObject> image;
  QPoint cursor;
  QwtPlotCurve* horizontalProfile;
  QwtPlotCurve* verticalProfile;
  QwtPlotMarker* horizontalMarker;
  QwtPlotMarker* verticalMarker;
  QwtPlotGrid* horizontalGrid;
  QwtPlotGrid* verticalGrid;
  QwtPlotPicker* horizontalPicker;
  QwtPlotPicker* verticalPicker;
};

#endif // PROFILEVIEW_H
