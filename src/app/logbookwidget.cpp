/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the log book                                      *
 *                                                                              *
 * modified: 2023-01-07                                                         *
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

#include "logbookwidget.h"
#include "ui_logbookwidget.h"
#include "appsettings.h"
#include "dialogs/addlogbookentrydialog.h"
#include "dialogs/logbookfilter1dialog.h"
#include <fitsbase/logbook/logbook.h>
#include <QDebug>
#include <QInputDialog>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QMenu>
#include <QTextStream>

LogbookWidget::LogbookWidget(QWidget *parent):QWidget(parent),
  ui(new Ui::LogbookWidget),
  logbook(nullptr),
  display(Table),
  updatingLists(false)
{
  ui->setupUi(this);
  contextMenu = new QMenu;
  contextMenu->addAction("Assign Project/Step",this,&LogbookWidget::assignProjectStep);

  connect(ui->projectBox,&QComboBox::currentTextChanged,this,&LogbookWidget::setProject);
  connect(ui->stepBox,&QComboBox::currentTextChanged,this,&LogbookWidget::setStep);
}

LogbookWidget::~LogbookWidget()
{
  delete ui;
}

void LogbookWidget::setLogbook(Logbook *log)
{
  if (logbook)
  {
    disconnect(logbook,nullptr,nullptr,nullptr);
  }
  logbook = log;
  connect(logbook,&Logbook::dataAdded,this,&LogbookWidget::addLastEntry);
  connect(logbook,&Logbook::dataChanged,this,&LogbookWidget::dataChanged);
  connect(logbook,&Logbook::activated,this,&LogbookWidget::setEnabled);
  setEnabled(logbook->isActive());
  rebuildLists();
  rebuild();
}

void LogbookWidget::dockLocationChanged(Qt::DockWidgetArea area)
{
}

void LogbookWidget::rebuild()
{
  switch (display)
  {
    case Table:
      rebuildTree();
      break;
    case TreeByDate:
      rebuildTreeByDate();
      break;
    case TreeByProject:
      rebuildTreeByProject();
      break;
    case TreeByProjectByDate:
      rebuildTreeByProjectByDate();
      break;
    case TreeByProjectByStep:
      rebuildTreeByProjectByStep();
      break;
  }
}

void LogbookWidget::setDisplay(Display d)
{
  AppSettings().setLogwidgetStyle(d);
  display = d;
  rebuild();
}




void LogbookWidget::dataChanged()
{
  rebuildLists();
  rebuild();
}

void LogbookWidget::rebuildLists()
{
  updatingLists = true;
  ui->projectBox->clear();
  for (const QString& p : logbook->getProjects()) ui->projectBox->addItem(p);
  ui->stepBox->clear();
  for (const QString& s : logbook->getSteps(ui->projectBox->currentText())) ui->stepBox->addItem(s);
  ui->projectBox->setCurrentIndex(0);
  ui->stepBox->setCurrentIndex(0);
  logbook->setProject(ui->projectBox->currentText());
  logbook->setStep(ui->stepBox->currentText());
  updatingLists = false;
}

void LogbookWidget::setProject(QString p)
{
  if (!updatingLists)
  {
    qDebug() << "setProject" << p;
    logbook->setProject(p);
    updatingLists = true;
    ui->stepBox->clear();
    for (const QString& s : logbook->getSteps(p)) ui->stepBox->addItem(s);
    updatingLists = false;
    ui->stepBox->setCurrentIndex(0);
  }
}

void LogbookWidget::setStep(QString s)
{
  if (!updatingLists)
  {
    qDebug() << "setStep" << s;
    logbook->setStep(s);
  }
}

void LogbookWidget::addLastEntry()
{
  const LogbookEntry& e = logbook->getLastEntry();
  if (e.isValid())
  {
    if (filter.check(e))
    {
      QTreeWidgetItem* item = nullptr;
      switch (display)
      {
        case Table:
          item = addEntry(e);
          break;
        case TreeByDate:
          item = addEntryByDate(e);
          break;
        case TreeByProject:
          item = addEntryByProject(e);
          break;
        case TreeByProjectByDate:
          item = addEntryByProjectByDate(e);
          break;
        case TreeByProjectByStep:
          item = addEntryByProjectByStep(e);
          break;
      }
      if (item) ui->logbookTreeWidget->scrollToItem(item);
    }
  }
}

QTreeWidgetItem* LogbookWidget::addEntry(const LogbookEntry &e)
{
  QStringList columns;
  QString tag = e.getTag();
  columns << e.timestamp.toString(Qt::ISODateWithMs)
          << e.getProject() << e.getStep()
          << tag << e.txt << QString::number(e.getId());
  QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
  ui->logbookTreeWidget->addTopLevelItem(item);
  return item;
}

QTreeWidgetItem* LogbookWidget::addEntryByDate(const LogbookEntry &e)
{
  QString tag = e.getTag();
  QString date = e.timestamp.date().toString(Qt::ISODate);
  QStringList columns;
  columns << e.timestamp.time().toString(Qt::ISODateWithMs)
          << e.getProject() << e.getStep()
          << tag << e.txt << QString::number(e.getId());
  QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
  QTreeWidgetItem* parent = findItem(date);
  if (!parent)
  {
    QStringList columns;
    columns << e.timestamp.date().toString(Qt::ISODate) << "" << "";
    parent = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    ui->logbookTreeWidget->addTopLevelItem(parent);
  }
  parent->addChild(item);
  return item;
}

QTreeWidgetItem* LogbookWidget::addEntryByProject(const LogbookEntry &e)
{
  QString tag = e.getTag();
  QStringList columns;
  columns << "" << e.timestamp.toString(Qt::ISODateWithMs)
          << e.getStep()
          << tag << e.txt << QString::number(e.getId());
  QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
  QTreeWidgetItem* parent = findItem(e.getProject());
  if (!parent)
  {
    QStringList columns;
    columns << e.getProject();
    parent = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    ui->logbookTreeWidget->addTopLevelItem(parent);
  }
  parent->addChild(item);
  return item;
}

QTreeWidgetItem* LogbookWidget::addEntryByProjectByDate(const LogbookEntry &e)
{
  QString tag = e.getTag();
  QString date = e.timestamp.date().toString(Qt::ISODate);
  QStringList columns;
  columns << "" << e.timestamp.toString(Qt::ISODateWithMs)
          << e.getStep()
          << tag << e.txt << QString::number(e.getId());
  QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
  QTreeWidgetItem* parent = findItem(e.getProject());
  if (!parent)
  {
    QStringList columns;
    columns << e.getProject();
    parent = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    ui->logbookTreeWidget->addTopLevelItem(parent);
  }
  parent = findAndCreateItem(parent,date);
  parent->addChild(item);
  return item;
}

QTreeWidgetItem* LogbookWidget::addEntryByProjectByStep(const LogbookEntry &e)
{
  QString tag = e.getTag();
  QStringList columns;
  columns << "" << e.timestamp.toString(Qt::ISODateWithMs)
          << tag << e.txt << QString::number(e.getId());
  QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
  QTreeWidgetItem* parent = findItem(e.getProject());
  if (!parent)
  {
    QStringList columns;
    columns << e.getProject();
    parent = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    ui->logbookTreeWidget->addTopLevelItem(parent);
  }
  parent = findAndCreateItem(parent,e.getStep());
  parent->addChild(item);
  return item;
}

void LogbookWidget::rebuildTree()
{
  ui->logbookTreeWidget->clear();
  ui->logbookTreeWidget->setColumnCount(3);
  QStringList header;
  header << "Date" << "Project" << "Step" << "Object" << "Text";
  ui->logbookTreeWidget->setHeaderLabels(header);
  ui->logbookTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  QList<QTreeWidgetItem*> items;
  for (const auto& e : logbook->getEntries(filter))
  {
    QStringList columns;
    QString tag = e.getTag();
    /* Note: the last column is not displayxed and contains the entry id */
    columns << e.timestamp.toString(Qt::ISODateWithMs)
            << e.getProject() << e.getStep()
            << tag << e.txt << QString::number(e.getId());
    items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns));
  }
  ui->logbookTreeWidget->insertTopLevelItems(0, items);
}

void LogbookWidget::rebuildTreeByDate()
{
  ui->logbookTreeWidget->clear();
  ui->logbookTreeWidget->setColumnCount(5);
  QStringList header;
  header << "Date" << "Project" << "Step" << "Object" << "Text";
  ui->logbookTreeWidget->setHeaderLabels(header);
  ui->logbookTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  QList<QTreeWidgetItem*> items;
  for (const auto& e : logbook->getEntries(filter))
  {
    QString tag = e.getTag();
    QString date = e.timestamp.date().toString(Qt::ISODate);
    QStringList columns;
    columns << e.timestamp.time().toString(Qt::ISODateWithMs)
            << e.getProject() << e.getStep()
            << tag << e.txt << QString::number(e.getId());
    QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    QTreeWidgetItem* parent = findAndCreateItem(items,date);
    parent->addChild(item);
  }
  ui->logbookTreeWidget->insertTopLevelItems(0,items);
}

void LogbookWidget::rebuildTreeByProject()
{
  ui->logbookTreeWidget->clear();
  ui->logbookTreeWidget->setColumnCount(5);
  QStringList header;
  header << "Project" << "Date" << "Step" << "Object" << "Text";
  ui->logbookTreeWidget->setHeaderLabels(header);
  ui->logbookTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  QList<QTreeWidgetItem*> items;
  for (const auto& e : logbook->getEntries(filter))
  {
    QString tag = e.getTag();
    QStringList columns;
    columns << "" << e.timestamp.toString(Qt::ISODateWithMs)
            << e.getStep()
            << tag << e.txt << QString::number(e.getId());
    QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    QTreeWidgetItem* parent = findAndCreateItem(items,e.getProject());
    parent->addChild(item);
  }
  ui->logbookTreeWidget->insertTopLevelItems(0,items);
}

void LogbookWidget::rebuildTreeByProjectByDate()
{
  ui->logbookTreeWidget->clear();
  ui->logbookTreeWidget->setColumnCount(5);
  QStringList header;
  header << "Project / Date" << "Time" << "Step" << "Object" << "Text";
  ui->logbookTreeWidget->setHeaderLabels(header);
  ui->logbookTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  QList<QTreeWidgetItem*> items;
  for (const auto& e : logbook->getEntries(filter))
  {
    QString tag = e.getTag();
    QString date = e.timestamp.date().toString(Qt::ISODate);
    QStringList columns;
    columns << "" << e.timestamp.time().toString(Qt::ISODateWithMs)
            << e.getStep()
            << tag << e.txt << QString::number(e.getId());
    QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    QTreeWidgetItem* parent = findAndCreateItem(items,e.getProject());
    parent = findAndCreateItem(parent,date);
    parent->addChild(item);
  }
  ui->logbookTreeWidget->insertTopLevelItems(0,items);
}

void LogbookWidget::rebuildTreeByProjectByStep()
{
  ui->logbookTreeWidget->clear();
  ui->logbookTreeWidget->setColumnCount(4);
  QStringList header;
  header << "Project / Step" << "Date" << "Object" << "Text";
  ui->logbookTreeWidget->setHeaderLabels(header);
  ui->logbookTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  QList<QTreeWidgetItem*> items;
  for (const auto& e : logbook->getEntries(filter))
  {
    QString tag = e.getTag();
    QStringList columns;
    columns << "" << e.timestamp.toString(Qt::ISODateWithMs)
            << tag << e.txt << QString::number(e.getId());
    QTreeWidgetItem* item = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    QTreeWidgetItem* parent = findAndCreateItem(items,e.getProject());
    parent = findAndCreateItem(parent,e.getStep());
    parent->addChild(item);
  }
  ui->logbookTreeWidget->insertTopLevelItems(0,items);
}

QTreeWidgetItem* LogbookWidget::findItem(const QString &text)
{
  for (int i=0;i<ui->logbookTreeWidget->topLevelItemCount();i++)
  {
    if (ui->logbookTreeWidget->topLevelItem(i)->text(0) == text) return ui->logbookTreeWidget->topLevelItem(i);
  }
  return nullptr;
}

QTreeWidgetItem* LogbookWidget::findItem(const QList<QTreeWidgetItem *> &list, const QString &text)
{
  for (QTreeWidgetItem* i : list)
  {
    if (i->text(0) == text) return i;
  }
  return nullptr;
}

QTreeWidgetItem* LogbookWidget::findItem(QTreeWidgetItem* parent, const QString &text)
{
  for (int i=0;i<parent->childCount();i++)
  {
    if (parent->child(i)->text(0) == text) return parent->child(i);
  }
  return nullptr;
}

QTreeWidgetItem* LogbookWidget::findAndCreateItem(QList<QTreeWidgetItem*>& list, const QString& text)
{
  QTreeWidgetItem* i = findItem(list,text);
  if (!i)
  {
    QStringList columns;
    columns << text;
    i = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    list.push_back(i);
  }
  return i;
}

QTreeWidgetItem* LogbookWidget::findAndCreateItem(QTreeWidgetItem* parent, const QString& text)
{
  QTreeWidgetItem* i = findItem(parent,text);
  if (!i)
  {
    QStringList columns;
    columns << text;
    i = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),columns);
    parent->addChild(i);
  }
  return i;
}



void LogbookWidget::assignProjectStep()
{
  QString project = ui->projectBox->currentText();
  QString step = ui->stepBox->currentText();
  QList<QTreeWidgetItem*> items = ui->logbookTreeWidget->selectedItems();
  for (QTreeWidgetItem* item : items)
  {
    int id = item->text(item->columnCount()-1).toLong();
    logbook->assignProject(id,project);
    logbook->assignStep(id,step);
  }
  rebuild();
}

void LogbookWidget::on_logbookTreeWidget_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->logbookTreeWidget->mapToGlobal(pos));
}

void LogbookWidget::on_addProjectButton_clicked()
{
  QString p = QInputDialog::getText(this,QApplication::applicationDisplayName(),"New project:");
  if (!p.isEmpty())
  {
    std::set<QString> set;
    for (int i=0;i<ui->projectBox->count();i++)
    {
      set.insert(ui->projectBox->itemText(i));
    }
    set.insert(p);
    updatingLists = true;
    ui->projectBox->clear();
    for (const QString& s : set) ui->projectBox->addItem(s);
    ui->projectBox->setCurrentText(p);
    logbook->setProject(p);
    updatingLists = false;
  }
}


void LogbookWidget::on_addStepButton_clicked()
{
  QString p = QInputDialog::getText(this,QApplication::applicationDisplayName(),"New step:");
  if (!p.isEmpty())
  {
    std::set<QString> set;
    for (int i=0;i<ui->stepBox->count();i++)
    {
      set.insert(ui->stepBox->itemText(i));
    }
    set.insert(p);
    updatingLists = true;
    ui->stepBox->clear();
    for (const QString& s : set) ui->stepBox->addItem(s);
    ui->stepBox->setCurrentText(p);
    logbook->setStep(p);
    updatingLists = false;
  }

}



void LogbookWidget::on_filterButton_clicked()
{
  LogbookFilter1Dialog d(logbook,this);
  if (d.exec())
  {
    filter = d.getFilter();rebuild();
  }
}

void LogbookWidget::on_addNoteButton_clicked()
{
  AddLogbookEntryDialog d(this);
  if (d.exec())
  {
    logbook->add(d.getType(),d.getImage(),d.getText());
  }
}
