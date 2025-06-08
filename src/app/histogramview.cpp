/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the histogram and associated controls             *
 *                                                                              *
 * modified: 2025-06-08                                                         *
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

#include "histogramview.h"
#include "ui_histogramview.h"
#include "appsettings.h"
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/qxt/qxtspanslider.h>
#include <qwt_scale_engine.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <QDebug>

static const int sliderrange = 1000;

HistogramView::HistogramView(QWidget *parent):QWidget(parent),
  ui(new Ui::HistogramView),
  updating(false),
  histmin(0),
  histmax(0)
{
  ui->setupUi(this);
  scalingSlider = new QxtSpanSlider(ui->controlFrame);
  scalingSlider->setObjectName(QString::fromUtf8("scalingSlider"));
  scalingSlider->setOrientation(Qt::Horizontal);

  ui->gridLayout->addWidget(scalingSlider, 1, 1, 1, 3);

  scalingSlider->setMinimum(0);
  scalingSlider->setMaximum(sliderrange);
  scalingSlider->setHandleMovementMode(QxtSpanSlider::NoCrossing);
  scalingSlider->setTickInterval(sliderrange/10);
  scalingSlider->setTickPosition(QSlider::TicksBelow);
  connect(scalingSlider,&QxtSpanSlider::spanChanged,this,&HistogramView::spanChanged);

  ui->forAllButton->setChecked(AppSettings().isImageScaleForAll());
  connect(ui->forAllButton,&QCheckBox::clicked,this,[](bool flag){AppSettings().setImageScaleForAll(flag);});

  grayCurve = new QwtPlotCurve("Gray");
  grayCurve->setPen(palette().color(QPalette::WindowText));
  grayCurve->attach(ui->chartWidget);
  ui->chartWidget->setAxisScaleEngine(QwtPlot::yLeft,new QwtLogScaleEngine());
  ui->chartWidget->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
  minMarker = new QwtPlotMarker();
  minMarker->setLineStyle(QwtPlotMarker::VLine);
  minMarker->setLinePen(Qt::blue,2.0,Qt::DashLine);
  minMarker->attach(ui->chartWidget);
  maxMarker = new QwtPlotMarker();
  maxMarker->setLineStyle(QwtPlotMarker::VLine);
  maxMarker->setLinePen(Qt::blue,2.0,Qt::DashLine);
  maxMarker->attach(ui->chartWidget);

  zoomer = new QwtPlotZoomer(ui->chartWidget->canvas());
  connect(zoomer,&QwtPlotZoomer::zoomed,this,&HistogramView::handleZoomed);

  grid = new QwtPlotGrid();
  grid->setPen(Qt::gray,0.0,Qt::DotLine);
  grid->attach(ui->chartWidget);

  plotPicker = new QwtPlotPicker(ui->chartWidget->xBottom,ui->chartWidget->yLeft,QwtPicker::NoRubberBand,QwtPicker::AlwaysOn,ui->chartWidget->canvas());
  plotPicker->setTrackerPen(palette().color(QPalette::WindowText));
  plotPicker->setRubberBandPen(palette().color(QPalette::WindowText));
  QwtPickerMachine* pickerMachine = new QwtPickerTrackerMachine();
  plotPicker->setStateMachine(pickerMachine);

  connect(ui->imageMinIntensity,&QLineEdit::returnPressed,this,&HistogramView::changeIntensity);
  connect(ui->imageMaxIntensity,&QLineEdit::returnPressed,this,&HistogramView::changeIntensity);
  connect(ui->imageScaleBox,&QComboBox::currentTextChanged,this,&HistogramView::changeIntensity);
  connect(ui->resetButton,&QPushButton::pressed,this,&HistogramView::reset);
}

HistogramView::~HistogramView()
{
  delete ui;
}

void HistogramView::setImage(std::shared_ptr<FitsObject> obj)
{
  bool keepscaling = ui->forAllButton->isChecked();
  if (!image) // fits image
    keepscaling = false;
  image = obj;
  if (image)
  {
    auto hist = image->getHistogram();
    double min,max;
    if (keepscaling)
    {
      min = minMarker->xValue(); //std::max(min,minMarker->xValue());
      max = maxMarker->xValue(); //std::min(max,maxMarker->xValue());
      histmin = std::min((double)hist.getMin(),min);
      histmax = std::max((double)hist.getMax(),max);
    }
    else
    {
      histmin = hist.getMin();
      histmax = hist.getMax();
      min = histmin;
      max = histmax;
    }
    if (histmin == histmax) histmax += 1.0;
    minMarker->setXValue(min);
    maxMarker->setXValue(max);
    ui->imageMinIntensity->setText(QString::number(min));
    ui->imageMaxIntensity->setText(QString::number(max));
//    ui->scalingSlider->setMinimum(std::min((double)hist.getMin(),min));
//    ui->scalingSlider->setMaximum(std::max((double)hist.getMax(),max));
    scalingSlider->setLowerValue(toSlider(min));
    scalingSlider->setUpperValue(toSlider(max));
  }
  ui->chartWidget->setAxisAutoScale(QwtPlot::yLeft,true);
  ui->chartWidget->setAxisAutoScale(QwtPlot::xBottom,true);
  redraw();
  zoomer->setZoomBase(true);
}

void HistogramView::redraw()
{
  QVector<QPointF> v;
  if (image)
  {
    auto hist = image->getHistogram();
    for (int i=0;i<hist.getBinCount();i++)
    {
      v.push_back(QPointF(hist.getX(i),std::max(static_cast<double>(hist.getGrayValue(i)),0.1)));
    }
  }
  grayCurve->setSamples(v);
  ui->chartWidget->replot();
}

int HistogramView::getImageScale() const
{
  return ui->imageScaleBox->currentIndex();
}

double HistogramView::getScaleMin() const
{
  return minMarker->xValue();
}

double HistogramView::getScaleMax() const
{
  return maxMarker->xValue();
}




void HistogramView::changeEvent(QEvent *event)
{
  QWidget::changeEvent(event);
  if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange)
  {
    grayCurve->setPen(palette().color(QPalette::WindowText));
    ui->chartWidget->replot();
  }
}



void HistogramView::reset()
{
  ui->forAllButton->setChecked(false);
  setImage(image);
}

void HistogramView::handleZoomed(const QRectF &rect)
{
  qDebug() << "zoomed: " << rect;
}

void HistogramView::spanChanged(int min, int max)
{
  if (!updating)
  {
    ui->imageMinIntensity->setText(QString::number(fromSlider(min)));
    ui->imageMaxIntensity->setText(QString::number(fromSlider(max)));
    changeIntensity();
  }
}

void HistogramView::changeIntensity()
{
  updating = true;
  double min = ui->imageMinIntensity->text().toDouble();
  double max = ui->imageMaxIntensity->text().toDouble();
  scalingSlider->setLowerValue(toSlider(min));
  scalingSlider->setUpperValue(toSlider(max));
  minMarker->setXValue(min);
  maxMarker->setXValue(max);
  ui->chartWidget->replot();
  updating = false;
  int scale = ui->imageScaleBox->currentIndex();
  emit imageScaleChanged(min,max,scale);
}

int HistogramView::toSlider(double v)
{
  return static_cast<int>((v - histmin)/(histmax-histmin) * sliderrange);
}

double HistogramView::fromSlider(int v)
{
  return static_cast<double>(v) / sliderrange * (histmax - histmin) + histmin;
}

