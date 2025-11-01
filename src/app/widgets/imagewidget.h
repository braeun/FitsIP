/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the actual image                                  *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QCursor>

class PixelList;
class QPainter;
class StarList;

class ImageWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ImageWidget(QWidget *parent = nullptr);

  virtual int heightForWidth(int w) const override;

  void setImage(const QImage& img, PixelList* pixels, StarList* stars);

  const QImage& getImage() const;

  void setZoom(int32_t z);

  void adjustSize();

  void clearAOI();

  void setAOI(const QRect& r);

  QRect getAOI() const;

signals:
  void contextMenuRequested(QPoint mousepos, QPoint pixel);
  void cursorMoved(QPoint p);
  void cursorSet(QPoint p);
  void aoiChanged(QRect r);
  void setPixel(QPoint p);

public slots:

protected:
  virtual void paintEvent(QPaintEvent* event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void contextMenuEvent(QContextMenuEvent *event) override;

private:
//  void drawRectangle(QPainter& p, QPoint start, QPoint stop);
  void drawAOI(QPainter& p);
  void drawPixelList(QPainter& p);
  void drawStarList(QPainter& p);

  QCursor cursor;
  QImage image;
  QImage origImage;
  QRect imageRect;
  QPoint dragStart;
  QPoint dragStop;
  QRect aoi;
  /* <0 zoom out, 0 fit */
  int zoom;
  double zoomFactor;
  PixelList* pixellist;
  StarList* starlist;
};

#endif // IMAGEWIDGET_H
