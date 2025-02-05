/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the profiles and associated controls              *
 *                                                                              *
 * modified: 2025-01-11                                                         *
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
#include <fitsbase/fitsobject.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_picker.h>
#include <QFileInfo>
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

  connect(ui->logYBox,&QCheckBox::toggled,this,&ProfileView::logYToggled);
  connect(ui->rangeBox,&QCheckBox::toggled,this,[this](bool){settingsChanged();});
  connect(ui->rangeSpinBox,qOverload<int>(&QSpinBox::valueChanged),this,[this](int){settingsChanged();});

  popupMenu = new QMenu();
  popupMenu->addAction("Save Profiles...",this,[this](){save();});
  ui->menuButton->setMenu(popupMenu);

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

void ProfileView::setImage(std::shared_ptr<FitsObject> obj)
{
  image = obj;
  if (image)
  {
    horizontal = image->getXProfile();
    vertical = image->getYProfile();
  }
  else
  {
    horizontal = Profile();
    vertical = Profile();
  }
  ui->horizontalProfileWidget->plot(horizontal,false);
  ui->verticalProfileWidget->plot(vertical,true);
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
//  if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange)
//  {
//  }
}




void ProfileView::redraw()
{
  horizontal = Profile();
  horizontal.setCursorX(cursor.x());
  horizontal.setCursorY(cursor.y());
  vertical = Profile();
  vertical.setCursorX(cursor.x());
  vertical.setCursorY(cursor.y());
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
        horizontal.push_back(QPointF(x,it.getAbs()));
        ++it;
      }
      int top = std::max(cursor.y()-r,0);
      int bottom = std::min(cursor.y()+r,(int)image->getImage()->getHeight());
      it = image->getImage()->getConstPixelIterator(cursor.x(),0);
      it += top * image->getImage()->getWidth();
      for (int y=top;y<bottom;y++)
      {
        vertical.push_back(QPointF(y,it.getAbs()));
        it += image->getImage()->getWidth();
      }
    }
    else
    {
      ConstPixelIterator it = image->getImage()->getConstPixelIterator(0,cursor.y());
      for (uint32_t x=0;x<image->getImage()->getWidth();x++)
      {
        horizontal.push_back(QPointF(x,it.getAbs()));
        ++it;
      }
      it = image->getImage()->getConstPixelIterator(cursor.x(),0);
      for (uint32_t y=0;y<image->getImage()->getHeight();y++)
      {
        vertical.push_back(QPointF(y,it.getAbs()));
        it += image->getImage()->getWidth();
      }
    }
    image->setXProfile(horizontal);
    image->setYProfile(vertical);
  }
  ui->horizontalProfileWidget->plot(horizontal,false);
  ui->verticalProfileWidget->plot(vertical,true);
}

void ProfileView::logYToggled(bool checked)
{
  ui->horizontalProfileWidget->setLogScale(checked);
  ui->verticalProfileWidget->setLogScale(checked);
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

void ProfileView::save()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_PROFILE,"Profiles (*.csv);;All files (*)");
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    QString fn = info.absolutePath() + "/" + info.completeBaseName() + "_hor." + info.suffix();
    horizontal.save(fn);
    fn = info.absolutePath() + "/" + info.completeBaseName() + "_ver." + info.suffix();
    vertical.save(fn);
  }

}
