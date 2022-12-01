/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the selected pixel list                           *
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

#ifndef PIXELLISTWIDGET_H
#define PIXELLISTWIDGET_H

#include <QWidget>

class PixelList;
class QMenu;

namespace Ui {
class PixelListWidget;
}

class PixelListWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PixelListWidget(QWidget *parent = nullptr);
  ~PixelListWidget();

private slots:
  void on_pixellistTable_customContextMenuRequested(const QPoint &pos);

private:
  void removeRows();
  void clear();

  Ui::PixelListWidget *ui;
  PixelList* pixellist;
  QMenu* contextMenu;
};

#endif // PIXELLISTWIDGET_H
