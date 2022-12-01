/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the histogram and associated controls             *
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

#include "histogramview.h"
#include "ui_histogramview.h"
#include "fileobject.h"
#include <qwt_scale_engine.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <QDebug>

HistogramView::HistogramView(QWidget *parent):QWidget(parent),
  ui(new Ui::HistogramView),
  updating(false)
{
  ui->setupUi(this);
  ui->scalingSlider->setHandleMovementMode(QxtSpanSlider::NoCrossing);
  connect(ui->scalingSlider,&QxtSpanSlider::spanChanged,this,&HistogramView::spanChanged);

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
}

HistogramView::~HistogramView()
{
  delete ui;
}

void HistogramView::setImage(std::shared_ptr<FileObject> obj)
{
  image = obj;
  if (image)
  {
    minMarker->setXValue(static_cast<double>(image->getHistogram()->getMin()));
    maxMarker->setXValue(static_cast<double>(image->getHistogram()->getMax()));
    ui->imageMinIntensity->setText(QString::number(static_cast<double>(image->getHistogram()->getMin())));
    ui->imageMaxIntensity->setText(QString::number(static_cast<double>(image->getHistogram()->getMax())));
    ui->scalingSlider->setMinimum(image->getHistogram()->getMin());
    ui->scalingSlider->setMaximum(image->getHistogram()->getMax());
    ui->scalingSlider->setLowerValue(image->getHistogram()->getMin());
    ui->scalingSlider->setUpperValue(image->getHistogram()->getMax());
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
    for (int32_t i=0;i<image->getHistogram()->getBinCount();i++)
    {
      v.push_back(QPointF(image->getHistogram()->getX(i),std::max(static_cast<double>(image->getHistogram()->getGrayValue(i)),0.1)));
    }
  }
  grayCurve->setSamples(v);
  ui->chartWidget->replot();
}

int32_t HistogramView::getImageScale() const
{
  return ui->imageScaleBox->currentIndex();
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



void HistogramView::handleZoomed(const QRectF &rect)
{
  qDebug() << "zoomed: " << rect;
}

void HistogramView::spanChanged(int min, int max)
{
  if (!updating)
  {
    ui->imageMinIntensity->setText(QString::number(min));
    ui->imageMaxIntensity->setText(QString::number(max));
    on_imageMinIntensity_returnPressed();
  }
}

void HistogramView::on_imageMinIntensity_returnPressed()
{
  updating = true;
  double min = ui->imageMinIntensity->text().toDouble();
  double max = ui->imageMaxIntensity->text().toDouble();
  ui->scalingSlider->setLowerValue(min);
  ui->scalingSlider->setUpperValue(max);
  minMarker->setXValue(static_cast<double>(min));
  maxMarker->setXValue(static_cast<double>(max));
  ui->chartWidget->replot();
  updating = false;
  int32_t scale = ui->imageScaleBox->currentIndex();
  emit imageScaleChanged(min,max,scale);
}

void HistogramView::on_imageMaxIntensity_returnPressed()
{
  on_imageMinIntensity_returnPressed();
}

void HistogramView::on_imageScaleBox_currentIndexChanged(int index)
{
  on_imageMinIntensity_returnPressed();
}
