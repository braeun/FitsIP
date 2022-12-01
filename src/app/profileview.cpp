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

#include "profileview.h"
#include "ui_profileview.h"
#include "fileobject.h"
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_picker.h>

ProfileView::ProfileView(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ProfileView)
{
  ui->setupUi(this);
  horizontalProfile = new QwtPlotCurve("X");
  horizontalProfile->setPen(palette().color(QPalette::WindowText));
  horizontalProfile->attach(ui->horizontalProfileWidget);
  verticalProfile = new QwtPlotCurve("Y");
  verticalProfile->setPen(palette().color(QPalette::WindowText));
  verticalProfile->attach(ui->verticalProfileWidget);

  horizontalMarker = new QwtPlotMarker();
  horizontalMarker->setLineStyle(QwtPlotMarker::VLine);
  horizontalMarker->setLinePen(Qt::blue,2.0,Qt::DashLine);
  horizontalMarker->attach(ui->horizontalProfileWidget);
  verticalMarker = new QwtPlotMarker();
  verticalMarker->setLineStyle(QwtPlotMarker::VLine);
  verticalMarker->setLinePen(Qt::blue,2.0,Qt::DashLine);
  verticalMarker->attach(ui->verticalProfileWidget);
  ui->horizontalProfileWidget->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);
  ui->verticalProfileWidget->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating,true);

  horizontalGrid = new QwtPlotGrid();
  horizontalGrid->setPen(Qt::gray,0.0,Qt::DotLine);
  horizontalGrid->attach(ui->horizontalProfileWidget);
  verticalGrid = new QwtPlotGrid();
  verticalGrid->setPen(Qt::gray,0.0,Qt::DotLine);
  verticalGrid->attach(ui->verticalProfileWidget);

  horizontalPicker = new QwtPlotPicker(ui->horizontalProfileWidget->xBottom,ui->horizontalProfileWidget->yLeft,QwtPicker::NoRubberBand,QwtPicker::AlwaysOn,ui->horizontalProfileWidget->canvas());
  horizontalPicker->setTrackerPen(palette().color(QPalette::WindowText));
  horizontalPicker->setRubberBandPen(palette().color(QPalette::WindowText));
  QwtPickerMachine* horizontalPickerMachine = new QwtPickerTrackerMachine();
  horizontalPicker->setStateMachine(horizontalPickerMachine);
  verticalPicker = new QwtPlotPicker(ui->verticalProfileWidget->xBottom,ui->verticalProfileWidget->yLeft,QwtPicker::NoRubberBand,QwtPicker::AlwaysOn,ui->verticalProfileWidget->canvas());
  verticalPicker->setTrackerPen(palette().color(QPalette::WindowText));
  verticalPicker->setRubberBandPen(palette().color(QPalette::WindowText));
  QwtPickerMachine* verticalPickerMachine = new QwtPickerTrackerMachine();
  verticalPicker->setStateMachine(verticalPickerMachine);

}

ProfileView::~ProfileView()
{
  delete ui;
}

void ProfileView::setImage(std::shared_ptr<FileObject> obj)
{
  image = obj;
  QVector<QPointF> h;
  QVector<QPointF> v;
  horizontalProfile->setSamples(h);
  verticalProfile->setSamples(v);
  horizontalMarker->setXValue(0);
  verticalMarker->setXValue(0);
  ui->horizontalProfileWidget->replot();
  ui->verticalProfileWidget->replot();
}

void ProfileView::updateCursor(QPoint p)
{
  if (ui->trackingButton->isChecked())
  {
    cursor = p;
    redraw();
  }
}

void ProfileView::setCursor(QPoint p)
{
  if (ui->onClickButton->isChecked())
  {
    cursor = p;
    redraw();
  }
}


void ProfileView::changeEvent(QEvent *event)
{
  QWidget::changeEvent(event);
  if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange)
  {
    horizontalProfile->setPen(palette().color(QPalette::WindowText));
    verticalProfile->setPen(palette().color(QPalette::WindowText));
    ui->horizontalProfileWidget->replot();
    ui->verticalProfileWidget->replot();
  }
}




void ProfileView::redraw()
{
  QVector<QPointF> h;
  QVector<QPointF> v;
  if (image)
  {
    ConstPixelIterator it = image->getImage()->getConstPixelIterator(0,cursor.y());
    for (uint32_t x=0;x<image->getImage()->getWidth();x++)
    {
      h.push_back(QPointF(x,it.getAbs()));
      ++it;
    }
    it = image->getImage()->getConstPixelIterator(cursor.x(),0);
    for (uint32_t y=0;y<image->getImage()->getHeight();y++)
    {
      v.push_back(QPointF(y,it.getAbs()));
      it += image->getImage()->getWidth();
    }
  }
  horizontalMarker->setXValue(cursor.x());
  verticalMarker->setXValue(cursor.y());
  horizontalProfile->setSamples(h);
  verticalProfile->setSamples(v);
  ui->horizontalProfileWidget->replot();
  ui->verticalProfileWidget->replot();
}




void ProfileView::on_logYBox_toggled(bool checked)
{
  if (checked)
  {
    ui->verticalProfileWidget->setAxisScaleEngine(QwtPlot::yLeft,new QwtLogScaleEngine());
    ui->horizontalProfileWidget->setAxisScaleEngine(QwtPlot::yLeft,new QwtLogScaleEngine());
  }
  else
  {
    ui->verticalProfileWidget->setAxisScaleEngine(QwtPlot::yLeft,new QwtLinearScaleEngine());
    ui->horizontalProfileWidget->setAxisScaleEngine(QwtPlot::yLeft,new QwtLinearScaleEngine());
  }
  ui->horizontalProfileWidget->replot();
  ui->verticalProfileWidget->replot();
}
