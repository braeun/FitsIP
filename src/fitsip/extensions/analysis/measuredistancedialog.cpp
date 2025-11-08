/********************************************************************************
 *                                                                              *
 * FitsIP - measure the distance dialog                                         *
 *                                                                              *
 * modified: 2025-11-08                                                         *
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

#include "measuredistancedialog.h"
#include "ui_measuredistancedialog.h"
#include <fitsip/core/pixellist.h>
#include <fitsip/core/db/database.h>
#include <QPushButton>
#include <QShowEvent>
#include <cmath>

MeasureDistanceDialog::MeasureDistanceDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::MeasureDistanceDialog)
{
  ui->setupUi(this);
  connect(ui->calculateButton,&QPushButton::clicked,this,&MeasureDistanceDialog::calculate);
  connect(ui->cameraBox,&QComboBox::currentTextChanged,this,&MeasureDistanceDialog::cameraSelected);
  connect(ui->telescopeBox,&QComboBox::currentTextChanged,this,&MeasureDistanceDialog::telescopeSelected);
}

MeasureDistanceDialog::~MeasureDistanceDialog()
{
  delete ui;
}

void MeasureDistanceDialog::setPixelList(PixelList* list)
{
  if (list->getPixels().size() >= 2)
  {
    p1x = list->getPixels()[0].x;
    p1y = list->getPixels()[0].y;
    p2x = list->getPixels()[1].x;
    p2y = list->getPixels()[1].y;
    ui->pixel1Label->setText(QString::asprintf("%.1f,%.1f",p1x,p1y));
    ui->pixel2Label->setText(QString::asprintf("%.1f,%.1f",p2x,p2y));
  }
}


void MeasureDistanceDialog::showEvent(QShowEvent *event)
{
  if (!event->spontaneous())
  {
    auto cameras = Database::get()->getCameraList();
    ui->cameraBox->clear();
    for (const auto& camera : cameras)
    {
      ui->cameraBox->addItem(camera);
    }
    auto telescopes = Database::get()->getTelescopeList();
    ui->telescopeBox->clear();
    for (const auto& telescope : telescopes)
    {
      ui->telescopeBox->addItem(telescope);
    }
  }
}


void MeasureDistanceDialog::cameraSelected(const QString& name)
{
  Camera camera = Database::get()->getCamera(name);
  ui->widthField->setText(QString::number(camera.getPixelwidth()));
  ui->heightField->setText(QString::number(camera.getPixelheight()));
}

void MeasureDistanceDialog::telescopeSelected(const QString& name)
{
  Telescope telescope = Database::get()->getTelescope(name);
  ui->focalLengthField->setText(QString::number(telescope.getF()));
}


void MeasureDistanceDialog::calculate()
{
  double dpx = std::fabs(p1x-p2x);
  double dpy = std::fabs(p1y-p2y);
  double distancepx = std::sqrt(dpx*dpx+dpy*dpy);
  double scalex = ui->widthField->text().toDouble();
  double scaley = ui->heightField->text().toDouble();
  double dmx = scalex * dpx;
  double dmy = scaley * dpy;
  double distancem = std::sqrt(dmx*dmx+dmy*dmy);
  ui->distancePixelLabel->setText(QString::asprintf("%.1f px",distancepx));
  ui->distanceMetricLabel->setText(QString::asprintf("%.1f µm",distancem));
  double f = ui->focalLengthField->text().toDouble();
  if (f > 0)
  {
    distancem /= 1000.0;
    double theta = std::atan(distancem/f) * 180.0 / M_PI;
    if (theta >= 1.0)
    {
      ui->distanceAngleLabel->setText(QString::asprintf("%.1f°",theta));
    }
    else
    {
      theta *= 60.0;
      if (theta >= 1.0)
      {
        ui->distanceAngleLabel->setText(QString::asprintf("%.1f'",theta));
      }
      else
      {
        theta *= 60.0;
        ui->distanceAngleLabel->setText(QString::asprintf("%.1f\"",theta));
      }
    }
  }
  else
  {
    ui->distanceAngleLabel->setText("Error: f <= 0");
  }
}

