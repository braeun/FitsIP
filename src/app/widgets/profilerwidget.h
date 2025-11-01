/********************************************************************************
 *                                                                              *
 * FitsIP - widget with the profiler table                                      *
 *                                                                              *
 * modified: 2025-11-01                                                         *
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

#ifndef PROFILERWIDGET_H
#define PROFILERWIDGET_H

#include <QWidget>

class ProfilerTableModel;
class QMenu;

namespace Ui {
class ProfilerWidget;
}

class ProfilerWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ProfilerWidget(QWidget *parent = nullptr);
  ~ProfilerWidget();

  ProfilerTableModel* getModel();

private:
  void exportData();
  void contextMenuRequested(const QPoint &pos);

  Ui::ProfilerWidget *ui;
  ProfilerTableModel* model;
  QMenu* contextMenu;
};

#endif // PROFILERWIDGET_H
