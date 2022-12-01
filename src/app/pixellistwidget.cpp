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

#include "pixellistwidget.h"
#include "ui_pixellistwidget.h"
#include <pixellist.h>
#include <QMenu>

PixelListWidget::PixelListWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::PixelListWidget)
{
  ui->setupUi(this);
  pixellist = PixelList::getGlobalInstance();
  ui->pixellistTable->setModel(pixellist);
  contextMenu = new QMenu();
  QAction* removeAction = contextMenu->addAction("Remove Row(s)");
  connect(removeAction,&QAction::triggered,this,&PixelListWidget::removeRows);
  QAction* clearAction = contextMenu->addAction("Clear");
  connect(clearAction,&QAction::triggered,this,&PixelListWidget::clear);
}

PixelListWidget::~PixelListWidget()
{
  delete ui;
}

void PixelListWidget::on_pixellistTable_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->pixellistTable->mapToGlobal(pos));
}

void PixelListWidget::removeRows()
{
  QModelIndexList list = ui->pixellistTable->selectionModel()->selectedRows();
  pixellist->removePixels(list);
}

void PixelListWidget::clear()
{
  pixellist->clear();
}
