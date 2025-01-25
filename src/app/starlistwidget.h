/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the selected/detected star list                   *
 *                                                                              *
 * modified: 2025-01-24                                                         *
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

#ifndef STARLISTWIDGET_H
#define STARLISTWIDGET_H

#include <QWidget>

class StarList;
class QMenu;

namespace Ui {
class StarListWidget;
}

class StarListWidget : public QWidget
{
  Q_OBJECT
public:
  explicit StarListWidget(QWidget *parent = nullptr);
  ~StarListWidget();

  void clear();

  void save();

  void load();

private:
  void on_starlistTable_customContextMenuRequested(const QPoint &pos);

  Ui::StarListWidget *ui;
  StarList* starlist;
  QMenu* contextMenu;
};

#endif // STARLISTWIDGET_H
