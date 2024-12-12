/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the selected/detected star list                   *
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

#include "starlistwidget.h"
#include "ui_starlistwidget.h"
#include <fitsbase/starlist.h>
#include <QMenu>

StarListWidget::StarListWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::StarListWidget)
{
  ui->setupUi(this);
  starlist = StarList::getGlobalInstance();
  ui->starlistTable->setModel(starlist);
  contextMenu = new QMenu();
}

StarListWidget::~StarListWidget()
{
  delete ui;
}

void StarListWidget::on_starlistTable_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->starlistTable->mapToGlobal(pos));
}
