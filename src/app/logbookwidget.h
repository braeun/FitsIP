/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the log book                                      *
 *                                                                              *
 * modified: 2022-12-04                                                         *
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

#ifndef LOGBOOKWIDGET_H
#define LOGBOOKWIDGET_H

#include <fitsbase/logbook/logbookfilter.h>
#include <QWidget>
#include <set>

class Logbook;
class LogbookEntry;
class QMenu;
class QTreeWidgetItem;

namespace Ui {
class LogbookWidget;
}

class LogbookWidget : public QWidget
{
  Q_OBJECT
public:
  enum Display { Table, TreeByDate, TreeByProject, TreeByProjectByDate, TreeByProjectByStep };

  explicit LogbookWidget(QWidget *parent = nullptr);
  ~LogbookWidget();

  void setLogbook(Logbook* log);

  void rebuild();

  void setDisplay(Display d);

public slots:
  void dockLocationChanged(Qt::DockWidgetArea area);

private slots:

  void on_logbookTreeWidget_customContextMenuRequested(const QPoint &pos);

  void on_addProjectButton_clicked();

  void on_addStepButton_clicked();

  void on_filterButton_clicked();

  void on_addNoteButton_clicked();

private:
  void assignProjectStep(QTreeWidgetItem* item, const QString& project, const QString& step);
  void assignProjectStep();
  void remove(QTreeWidgetItem* item);
  void remove();
  void editEntry();
  void dataChanged();
  void rebuildLists();
  void setProject(QString p);
  void setStep(QString s);
  void addLastEntry();
  QTreeWidgetItem* addEntry(const LogbookEntry& e);
  QTreeWidgetItem* addEntryByDate(const LogbookEntry& e);
  QTreeWidgetItem* addEntryByProject(const LogbookEntry& e);
  QTreeWidgetItem* addEntryByProjectByDate(const LogbookEntry& e);
  QTreeWidgetItem* addEntryByProjectByStep(const LogbookEntry& e);
  void rebuildTree();
  void rebuildTreeByDate();
  void rebuildTreeByProject();
  void rebuildTreeByProjectByDate();
  void rebuildTreeByProjectByStep();
  QTreeWidgetItem* findItem(const QString& text);
  QTreeWidgetItem* findItem(const QList<QTreeWidgetItem*>& list, const QString& text);
  QTreeWidgetItem* findItem(QTreeWidgetItem* parent, const QString& text);
  QTreeWidgetItem* findAndCreateItem(QList<QTreeWidgetItem*>& list, const QString& text);
  QTreeWidgetItem* findAndCreateItem(QTreeWidgetItem* parent, const QString& text);

  Ui::LogbookWidget *ui;
  QMenu* contextMenu;
  QAction* editEntryAction;
  Logbook* logbook;
  LogbookFilter filter;
  Display display;
  bool updatingLists;
  bool batchUpdating;
};

#endif // LOGBOOKWIDGET_H
