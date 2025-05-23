/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the actual image                                  *
 *                                                                              *
 * modified: 2025-03-15                                                         *
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

#include "imagewidget.h"
#include "appsettings.h"
#include <fitsbase/pixellist.h>
#include <fitsbase/starlist.h>
#include <QPixmap>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QDebug>
#include <algorithm>

ImageWidget::ImageWidget(QWidget *parent): QWidget(parent),
  pixellist(nullptr),
  starlist(nullptr)
{
  setMouseTracking(true);
  QPixmap p("://resources/cursors/crosshair.png");
  cursor = QCursor(p);
  setCursor(cursor);
  setBackgroundRole(QPalette::Base);
  setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  setZoom(0);
}

int ImageWidget::heightForWidth(int w) const
{
  if (image.isNull()) return -1;
  return static_cast<int>(static_cast<double>(image.height())/image.width()*w);
}

void ImageWidget::setImage(const QImage& img, PixelList* pixels, StarList* stars)
{
  if (pixellist)
  {
    disconnect(pixellist,nullptr,this,nullptr);
  }
  pixellist = pixels;
  if (pixellist)
  {
    connect(pixellist,&PixelList::rowsInserted,this,[this](const QModelIndex&,int,int){repaint();});
    connect(pixellist,&PixelList::rowsRemoved,this,[this](const QModelIndex&,int,int){repaint();});
    connect(pixellist,&PixelList::layoutChanged,this,[this](){repaint();});
  }
  if (starlist)
  {
    disconnect(starlist,nullptr,this,nullptr);
  }
  starlist = stars;
  if (starlist)
  {
    connect(starlist,&StarList::rowsInserted,this,[this](const QModelIndex&,int,int){repaint();});
    connect(starlist,&StarList::rowsRemoved,this,[this](const QModelIndex&,int,int){repaint();});
    connect(starlist,&StarList::layoutChanged,this,[this](){repaint();});
  }
  origImage = img;
  if (zoom < -1)
  {
    image = origImage.scaled(origImage.width()/abs(zoom),origImage.height()/abs(zoom));
    zoomFactor = 1.0 / abs(zoom);
  }
  else if (zoom > 1)
  {
    image = origImage.scaled(origImage.width()*abs(zoom),origImage.height()*abs(zoom));
    zoomFactor = 1.0 * abs(zoom);
  }
  else
  {
    image = origImage;
    zoomFactor = 1.0;
  }
  if (zoom != 0) adjustSize();
  update();
}

const QImage& ImageWidget::getImage() const
{
  return origImage;
}

void ImageWidget::setZoom(int32_t z)
{
  zoom = z;
  if (!origImage.isNull()) setImage(origImage,pixellist,starlist);
}

void ImageWidget::adjustSize()
{
  if (!image.isNull())
  {
    resize(image.width(),image.height());
  }
}

void ImageWidget::clearAOI()
{
  aoi = QRect();
  repaint();
  emit aoiChanged(aoi);
}

void ImageWidget::setAOI(const QRect& r)
{
  aoi = r;
  repaint();
  emit aoiChanged(aoi);
}

QRect ImageWidget::getAOI() const
{
  return aoi;
}

void ImageWidget::paintEvent(QPaintEvent* /*event*/)
{
  AppSettings settings;
  QPainter p(this);
  p.fillRect(0,0,width(),height(),palette().color(QPalette::Shadow));
  if (!image.isNull())
  {
    double scale = std::min(static_cast<double>(width()) / image.width(),static_cast<double>(height()) / image.height());
    if (scale > 1) scale = 1;
    int w = static_cast<int>(image.width() * scale);
    int h = static_cast<int>(image.height() * scale);
    imageRect = QRect((width()-w)/2,(height()-h)/2,w,h);
    p.drawImage(imageRect,image);
//    drawRectangle(p,dragStart,dragStop);
    drawAOI(p);
    if (pixellist && settings.isShowPixellist()) drawPixelList(p);
    if (starlist && settings.isShowStarlist()) drawStarList(p);
  }
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MouseButton::LeftButton)
  {
    if (imageRect.contains(event->pos()))
    {
      dragStart = event->pos();
      dragStop = event->pos();
      aoi = QRect();
    }
  }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::MouseButton::LeftButton)
  {
    if (dragStart == dragStop)
    {
      dragStart = QPoint();
      dragStop = QPoint();
    }
    repaint();
    if (!image.isNull() && imageRect.contains(event->pos()))
    {
      double scale = static_cast<double>(image.width()) / static_cast<double>(imageRect.width());
      QPoint p = (event->pos() - imageRect.topLeft()) * scale;
      emit cursorSet(p);
      if (dragStart == dragStop) emit setPixel(p/zoomFactor);
    }
  }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!image.isNull() && imageRect.contains(event->pos()))
  {
    double scale = static_cast<double>(image.width()) / static_cast<double>(imageRect.width());
    if (event->buttons().testFlag(Qt::MouseButton::LeftButton))
    {
      dragStop = event->pos();
      QPoint tl = (dragStart - imageRect.topLeft()) * scale / zoomFactor;
      QPoint br = (dragStop - imageRect.topLeft()) * scale / zoomFactor;
      aoi = QRect(tl,br).normalized();
      repaint();
      emit aoiChanged(aoi);
    }
    QPoint p = (event->pos() - imageRect.topLeft()) * scale / zoomFactor;
    emit cursorMoved(p);
  }
}

//void ImageWidget::drawRectangle(QPainter& p, QPoint start, QPoint stop)
//{
//  p.save();
//  p.setPen(Qt::yellow);
//  if (!(start.isNull() || stop.isNull()))
//  {
//    p.drawRect(start.x(),start.y(),stop.x()-start.x(),stop.y()-start.y());
//  }
//  p.restore();
//}

void ImageWidget::drawAOI(QPainter &p)
{
  if (aoi.isEmpty()) return;
  double scale = static_cast<double>(image.width()) / static_cast<double>(imageRect.width());
  QPoint tl = aoi.topLeft() / scale * zoomFactor + imageRect.topLeft();
  QPoint br = aoi.bottomRight() / scale * zoomFactor + imageRect.topLeft();
  p.save();
  p.setPen(Qt::yellow);
  p.drawRect(QRect(tl,br));
  p.restore();
}

void ImageWidget::drawPixelList(QPainter& p)
{
  p.save();
  p.setPen(Qt::red);
  double scale = static_cast<double>(image.width()) / static_cast<double>(imageRect.width());
  for (const Pixel& pixel : pixellist->getPixels())
  {
    QPoint pt = QPoint(pixel.x,pixel.y) / scale * zoomFactor + imageRect.topLeft();
    p.drawLine(pt-QPoint(5,0),pt+QPoint(5,0));
    p.drawLine(pt-QPoint(0,5),pt+QPoint(0,5));
  }
  p.restore();
}

void ImageWidget::drawStarList(QPainter& p)
{
  p.save();
  p.setPen(Qt::green);
  double scale = static_cast<double>(image.width()) / static_cast<double>(imageRect.width());
  for (const Star& star :starlist->getStars())
  {
    QPointF pt = QPointF(star.getX(),star.getY()) / scale * zoomFactor + imageRect.topLeft();
    double r = star.getFWHM() / 2 / scale;
    if (r < 1) r = 2;
    p.drawEllipse(pt,r,r);
  }
  p.restore();
}



