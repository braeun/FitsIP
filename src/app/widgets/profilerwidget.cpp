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

#include "profilerwidget.h"
#include "ui_profilerwidget.h"
#include "appsettings.h"
#include "profilertablemodel.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMenu>

ProfilerWidget::ProfilerWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ProfilerWidget)
{
  ui->setupUi(this);
  connect(ui->profilerTableView,&QTableView::customContextMenuRequested,this,&ProfilerWidget::contextMenuRequested);
  connect(ui->actionExport,&QAction::triggered,this,&ProfilerWidget::exportData);
  contextMenu = new QMenu;
  contextMenu->addAction(ui->actionExport);
  contextMenu->addSeparator();
  contextMenu->addAction(ui->actionClear);
  model = new ProfilerTableModel(this);
  connect(model,&QAbstractTableModel::layoutChanged,this,[this](){ui->profilerTableView->scrollToBottom();});
  ui->profilerTableView->setModel(model);
  ui->profilerTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  connect(ui->actionClear,&QAction::triggered,model,&ProfilerTableModel::clear);
}

ProfilerWidget::~ProfilerWidget()
{
  delete ui;
}

ProfilerTableModel* ProfilerWidget::getModel()
{
  return model;
}

void ProfilerWidget::exportData()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,"CSV  (*.csv)");
  if (!fn.isNull())
  {
    QFile file(fn);
    if (file.open(QFile::WriteOnly))
    {
      QTextStream s(&file);
      for (const ProfilerEntry& e : model->getEntries())
      {
        s << "\"" << e.profiler << "\",\"" << e.image << "\"," << e.w << "," << e.h << "," << e.t << ",\"" << e.notes << "\"\n";
      }
      s.flush();
    }
    file.close();
  }
}

void ProfilerWidget::contextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->profilerTableView->mapToGlobal(pos));
}
