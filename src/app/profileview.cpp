/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the profiles and associated controls              *
 *                                                                              *
 * modified: 2024-11-23                                                         *
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
#include "appsettings.h"
#include <fitsbase/fileobject.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_picker.h>
#include <QSettings>


static const char* PROFILEVIEW_LOGY = "profile/logy";
static const char* PROFILEVIEW_TRACK = "profile/track";
static const char* PROFILEVIEW_RANGE = "profile/range";
static const char* PROFILEVIEW_RANGE_LENGTH = "profile/rangelength";


ProfileView::ProfileView(QWidget *parent):QWidget(parent),
  ui(new Ui::ProfileView),
  clickEndsTracking(AppSettings().isProfileStopTracking())
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

  connect(ui->logYBox,&QCheckBox::toggled,this,&ProfileView::logYToggled);
  connect(ui->rangeBox,&QCheckBox::toggled,this,[this](bool){settingsChanged();});
  connect(ui->rangeSpinBox,qOverload<int>(&QSpinBox::valueChanged),this,[this](int){settingsChanged();});

  QSettings settings;
  ui->logYBox->setChecked(settings.value(PROFILEVIEW_LOGY,false).toBool());
  ui->trackingBox->setChecked(settings.value(PROFILEVIEW_TRACK,false).toBool());
  ui->rangeBox->setChecked(settings.value(PROFILEVIEW_RANGE,false).toBool());
  ui->rangeSpinBox->setValue(settings.value(PROFILEVIEW_RANGE_LENGTH,200).toInt());
}

ProfileView::~ProfileView()
{
  delete ui;
}

bool ProfileView::getClickEndsTracking() const
{
  return clickEndsTracking;
}

void ProfileView::setClickEndsTracking(bool newClickEndsTracking)
{
  clickEndsTracking = newClickEndsTracking;
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
  if (ui->trackingBox->isChecked())
  {
    cursor = p;
    redraw();
  }
}

void ProfileView::setCursor(QPoint p)
{
  cursor = p;
  redraw();
  if (clickEndsTracking) ui->trackingBox->setChecked(false);
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
    if (ui->rangeBox->isChecked())
    {
      int r = ui->rangeSpinBox->value() / 2;
      int left = std::max(cursor.x()-r,0);
      int right = std::min(cursor.x()+r,(int)image->getImage()->getWidth());
      ConstPixelIterator it = image->getImage()->getConstPixelIterator(0,cursor.y());
      it += left;
      for (int x=left;x<right;x++)
      {
        h.push_back(QPointF(x,it.getAbs()));
        ++it;
      }
      int top = std::max(cursor.y()-r,0);
      int bottom = std::min(cursor.y()+r,(int)image->getImage()->getHeight());
      it = image->getImage()->getConstPixelIterator(cursor.x(),0);
      it += top * image->getImage()->getWidth();
      for (int y=top;y<bottom;y++)
      {
        v.push_back(QPointF(y,it.getAbs()));
        it += image->getImage()->getWidth();
      }
    }
    else
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
  }
  horizontalMarker->setXValue(cursor.x());
  verticalMarker->setXValue(cursor.y());
  horizontalProfile->setSamples(h);
  verticalProfile->setSamples(v);
  ui->horizontalProfileWidget->replot();
  ui->verticalProfileWidget->replot();
}

void ProfileView::logYToggled(bool checked)
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
  QSettings().setValue(PROFILEVIEW_LOGY,checked);
}

void ProfileView::settingsChanged()
{
  QSettings settings;
  QSettings().setValue(PROFILEVIEW_TRACK,ui->trackingBox->isChecked());
  QSettings().setValue(PROFILEVIEW_RANGE,ui->rangeBox->isChecked());
  QSettings().setValue(PROFILEVIEW_RANGE_LENGTH,ui->rangeSpinBox->value());
  redraw();
}

