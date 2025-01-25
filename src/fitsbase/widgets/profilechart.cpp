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

#include "profilechart.h"
#include "../profile.h"
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <QEvent>

ProfileChart::ProfileChart(QWidget* parent):QwtPlot(parent)
{
  profile = new QwtPlotCurve("Profile");
  profile->setPen(palette().color(QPalette::WindowText));
  profile->attach(this);

  marker = new QwtPlotMarker();
  marker->setLineStyle(QwtPlotMarker::VLine);
  marker->setLinePen(Qt::blue,2.0,Qt::DashLine);
  marker->attach(this);
  axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);

  grid = new QwtPlotGrid();
  grid->setPen(Qt::gray,0.0,Qt::DotLine);
  grid->attach(this);

  picker = new QwtPlotPicker(QwtPlot::xBottom,QwtPlot::yLeft,QwtPicker::NoRubberBand,QwtPicker::AlwaysOn,canvas());
  picker->setTrackerPen(palette().color(QPalette::WindowText));
  picker->setRubberBandPen(palette().color(QPalette::WindowText));
  QwtPickerMachine* pickerMachine = new QwtPickerTrackerMachine();
  picker->setStateMachine(pickerMachine);

//  zoomer = new QwtPlotZoomer(canvas());
}

void ProfileChart::plot(const Profile& profile, bool vertical)
{
  this->profile->setSamples(profile);
  marker->setXValue(vertical?profile.getCursorY():profile.getCursorX());
  replot();
}

void ProfileChart::setLogScale(bool flag)
{
  if (flag)
  {
    setAxisScaleEngine(QwtPlot::yLeft,new QwtLogScaleEngine());
  }
  else
  {
    setAxisScaleEngine(QwtPlot::yLeft,new QwtLinearScaleEngine());
  }
  replot();
}

void ProfileChart::changeEvent(QEvent *event)
{
  QWidget::changeEvent(event);
  if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange)
  {
    profile->setPen(palette().color(QPalette::WindowText));
    replot();
  }
}

