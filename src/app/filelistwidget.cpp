/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing a file list                                       *
 *                                                                              *
 * modified: 2022-11-21                                                         *
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

#include "filelistwidget.h"
#include "ui_filelistwidget.h"
#include "appsettings.h"
#include <filelist.h>
#include <QMenu>

FileListWidget::FileListWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FileListWidget)
{
  ui->setupUi(this);
  fileList = new FileList(this);
  ui->fileList->setModel(fileList);
  contextMenu = new QMenu();
  QAction* open = contextMenu->addAction("Open Selected");
  connect(open,&QAction::triggered,[=](){emit openSelected();});
  contextMenu->addSeparator();
  QAction* load = contextMenu->addAction("Load...");
  connect(load,&QAction::triggered,this,&FileListWidget::load);
  QAction* save = contextMenu->addAction("Save...");
  connect(save,&QAction::triggered,this,&FileListWidget::save);
  contextMenu->addSeparator();
  QAction* remove = contextMenu->addAction("Remove Selected");
  connect(remove,&QAction::triggered,this,&FileListWidget::removeFiles);
  contextMenu->addSeparator();
  QAction* clear = contextMenu->addAction("Clear");
  connect(clear,&QAction::triggered,this,&FileListWidget::clear);
}

FileListWidget::~FileListWidget()
{
  delete ui;
}

FileList* FileListWidget::getFileList()
{
  return fileList;
}

void FileListWidget::clear()
{
  fileList->clear();
}

void FileListWidget::load()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_FILELIST,"File list (*.lst);;All files (*)");
  if (!fn.isNull())
  {
    fileList->load(fn);
  }
}

void FileListWidget::save()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_FILELIST,"File list (*.lst);;All files (*)");
  if (!fn.isNull())
  {
    fileList->save(fn);
  }
}

int FileListWidget::next()
{
  int n = (ui->fileList->currentIndex().row() + 1) % fileList->rowCount();
  ui->fileList->setCurrentIndex(fileList->index(n,0,QModelIndex()));
  return n;
}

int FileListWidget::previous()
{
  int n = ui->fileList->currentIndex().row() - 1;
  if (n < 0) n += fileList->rowCount();
  ui->fileList->setCurrentIndex(fileList->index(n,0,QModelIndex()));
  return n;
}

std::vector<QFileInfo> FileListWidget::getSelection() const
{
  std::vector<QFileInfo> filelist;
  for (int32_t row=0;row<fileList->rowCount();row++)
  {
    if (ui->fileList->selectionModel()->isRowSelected(row,QModelIndex()))
    {
      filelist.push_back(fileList->getFiles()[row]);
    }
  }
  return filelist;
}




void FileListWidget::on_fileList_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->fileList->mapToGlobal(pos));
}

void FileListWidget::on_fileList_doubleClicked(const QModelIndex &index)
{
  emit openSelected();
}

void FileListWidget::removeFiles()
{
  std::vector<QFileInfo> filelist;
  for (int32_t row=0;row<fileList->rowCount();row++)
  {
    if (!ui->fileList->selectionModel()->isRowSelected(row,QModelIndex()))
    {
      filelist.push_back(fileList->getFiles()[row]);
    }
  }
  fileList->setFiles(filelist);
}

