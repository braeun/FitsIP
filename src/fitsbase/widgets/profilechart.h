/********************************************************************************
 *                                                                              *
 * FitsIP - chart to display a profile                                          *
 *                                                                              *
 * modified: 2025-01-12                                                         *
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

#ifndef PROFILECHART_H
#define PROFILECHART_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

class Profile;
class QwtPlotGrid;
class QwtPlotPicker;
class QwtPlotZoomer;

class ProfileChart: public QwtPlot
{
public:
  ProfileChart(QWidget* parent=nullptr);

  void plot(const Profile& profile, bool vertical);

  void setLogScale(bool flag);

protected:
  void changeEvent(QEvent* event);

private:
  QwtPlotCurve* profile;
  QwtPlotMarker* marker;
  QwtPlotGrid* grid;
  QwtPlotPicker* picker;
  QwtPlotZoomer* zoomer;
};

#endif // PROFILECHART_H
