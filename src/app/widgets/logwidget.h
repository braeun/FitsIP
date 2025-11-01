/********************************************************************************
 *                                                                              *
 * FitsIP - widget with the general logging console                             *
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

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

class QMenu;

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
  Q_OBJECT

public:
  explicit LogWidget(QWidget *parent = nullptr);
  ~LogWidget();

  void add(const QtMsgType type, const QMessageLogContext& context, const QString &message);

  void writeStdOut(const QString& s);

  void writeStdErr(const QString& s);

  void dockLocationChanged(Qt::DockWidgetArea area);

private:
  void contextMenuRequested(const QPoint &pos);
  void save();

  Ui::LogWidget *ui;
  QMenu* contextMenu;
};

#endif // LOGWIDGET_H
