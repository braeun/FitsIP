/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the selected pixel list                           *
 *                                                                              *
 * modified: 2024-12-13                                                         *
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

#include "pixellistwidget.h"
#include "ui_pixellistwidget.h"
#include "appsettings.h"
#include <fitsbase/pixellist.h>
#include <QMenu>

PixelListWidget::PixelListWidget(QWidget *parent):QWidget(parent),
  ui(new Ui::PixelListWidget),
  pixellist(nullptr)
{
  ui->setupUi(this);
  contextMenu = new QMenu();
  QAction* load = contextMenu->addAction("Load...");
  connect(load,&QAction::triggered,this,&PixelListWidget::load);
  QAction* save = contextMenu->addAction("Save...");
  connect(save,&QAction::triggered,this,&PixelListWidget::save);
  contextMenu->addSeparator();
  QAction* removeAction = contextMenu->addAction("Remove Row(s)");
  connect(removeAction,&QAction::triggered,this,&PixelListWidget::removeRows);
  QAction* clearAction = contextMenu->addAction("Clear");
  connect(clearAction,&QAction::triggered,this,&PixelListWidget::clear);
  contextMenu->addSeparator();
  QAction* findStarsAction = contextMenu->addAction("Find Stars...");
  connect(findStarsAction,&QAction::triggered,this,[this](){ emit findStars(); });
}

PixelListWidget::~PixelListWidget()
{
  delete ui;
}

void PixelListWidget::on_pixellistTable_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->pixellistTable->mapToGlobal(pos));
}

void PixelListWidget::setPixelList(PixelList* list)
{
  pixellist = list;
  QItemSelectionModel *m = ui->pixellistTable->selectionModel();
  ui->pixellistTable->setModel(list);
  delete m;
}

void PixelListWidget::clear()
{
  pixellist->clear();
}

void PixelListWidget::load()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_PIXELLIST,"File list (*.lst);;All files (*)");
  if (!fn.isNull())
  {
    pixellist->load(fn);
  }
}

void PixelListWidget::save()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_PIXELLIST,"File list (*.lst);;All files (*)");
  if (!fn.isNull())
  {
    pixellist->save(fn);
  }
}

void PixelListWidget::removeRows()
{
  QModelIndexList list = ui->pixellistTable->selectionModel()->selectedRows();
  pixellist->removePixels(list);
}

