/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the log book                                      *
 *                                                                              *
 * modified: 2022-11-29                                                         *
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

#include <logbook/logbookfilter.h>
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
  enum Display { Table, TreeByDate, TreeByProject, TreeByProjectByDate };

  explicit LogbookWidget(QWidget *parent = nullptr);
  ~LogbookWidget();

  void setLogbook(Logbook* log);

  void rebuild();

  void setDisplay(Display d);

  void exportToFile(const QString& filename);

public slots:
  void dockLocationChanged(Qt::DockWidgetArea area);

private slots:

  void assignProjectStep();

  void on_logbookTreeWidget_customContextMenuRequested(const QPoint &pos);

  void on_addProjectButton_clicked();

  void on_addStepButton_clicked();

  void on_filterButton_clicked();

  void on_addNoteButton_clicked();

private:
  void dataChanged();
  void rebuildLists();
  void setProject(QString p);
  void setStep(QString s);
  void addLastEntry();
  QString getTag(const LogbookEntry& e);
  void addEntry(const LogbookEntry& e);
  void addEntryByDate(const LogbookEntry& e);
  void addEntryByProject(const LogbookEntry& e);
  void addEntryByProjectByDate(const LogbookEntry& e);
  void rebuildTree();
  void rebuildTreeByDate();
  void rebuildTreeByProject();
  void rebuildTreeByProjectByDate();
  QTreeWidgetItem* findItem(const QString& text);
  QTreeWidgetItem* findItem(const QList<QTreeWidgetItem*>& list, const QString& text);
  QTreeWidgetItem* findItem(QTreeWidgetItem* parent, const QString& text);
  QTreeWidgetItem* findAndCreateItem(QList<QTreeWidgetItem*>& list, const QString& text);
  QTreeWidgetItem* findAndCreateItem(QTreeWidgetItem* parent, const QString& text);

  Ui::LogbookWidget *ui;
  QMenu* contextMenu;
  Logbook* logbook;
  LogbookFilter filter;
  Display display;
  bool updatingLists;

};

#endif // LOGBOOKWIDGET_H
