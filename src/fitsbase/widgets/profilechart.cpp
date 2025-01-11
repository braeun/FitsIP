#include "profilechart.h"
#include "../profile.h"
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_picker.h>

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

