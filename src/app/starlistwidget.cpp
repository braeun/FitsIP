/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the selected/detected star list                   *
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

#include "starlistwidget.h"
#include "ui_starlistwidget.h"
#include "appsettings.h"
#include <fitsip/core/starlist.h>
#include <QMenu>

StarListWidget::StarListWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::StarListWidget)
{
  ui->setupUi(this);
  contextMenu = new QMenu();
  QAction* load = contextMenu->addAction("Load...");
  connect(load,&QAction::triggered,this,&StarListWidget::load);
  QAction* save = contextMenu->addAction("Save...");
  connect(save,&QAction::triggered,this,&StarListWidget::save);
}

StarListWidget::~StarListWidget()
{
  delete ui;
}

void StarListWidget::setStarList(StarList* list)
{
  starlist = list;
  QItemSelectionModel *m = ui->starlistTable->selectionModel();
  ui->starlistTable->setModel(list);
  delete m;
}

void StarListWidget::clear()
{
  starlist->clear();
}

void StarListWidget::load()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_STARLIST,"File list (*.lst);;All files (*)");
  if (!fn.isNull())
  {
    starlist->load(fn);
  }
}

void StarListWidget::save()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_STARLIST,"File list (*.lst);;All files (*)");
  if (!fn.isNull())
  {
    starlist->save(fn);
  }
}

void StarListWidget::on_starlistTable_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->starlistTable->mapToGlobal(pos));
}
