/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing a file list                                       *
 *                                                                              *
 * modified: 2025-02-22                                                         *
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
#include <fitsip/core/filelist.h>
#include <QAbstractItemView>
#include <QClipboard>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>

FileListWidget::FileListWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FileListWidget)
{
  ui->setupUi(this);

  connect(ui->fileList,&QAbstractItemView::customContextMenuRequested,this,&FileListWidget::contextMenuRequested);
  connect(ui->fileList,&QAbstractItemView::doubleClicked,this,&FileListWidget::doubleClicked);
  contextMenu = new QMenu();
  QAction* open = contextMenu->addAction("Open Selected");
  connect(open,&QAction::triggered,this,[this](){emit openSelected();});
  QAction* copyToClipboard = contextMenu->addAction("Copy Selection to Clipboard");
  connect(copyToClipboard,&QAction::triggered,this,&FileListWidget::copySelectionToClipboard);
  QAction* copyFiles = contextMenu->addAction("Copy Files...");
  connect(copyFiles,&QAction::triggered,this,&FileListWidget::copyFiles);
  contextMenu->addSeparator();
  QAction* load = contextMenu->addAction("Load...");
  connect(load,&QAction::triggered,this,&FileListWidget::load);
  QAction* append = contextMenu->addAction("Append...");
  connect(append,&QAction::triggered,this,&FileListWidget::append);
  QAction* save = contextMenu->addAction("Save...");
  connect(save,&QAction::triggered,this,&FileListWidget::save);
  contextMenu->addSeparator();
  QAction* search = contextMenu->addAction("Search...");
  connect(search,&QAction::triggered,this,&FileListWidget::search);
  contextMenu->addSeparator();
  QAction* remove = contextMenu->addAction("Remove Selected");
  connect(remove,&QAction::triggered,this,&FileListWidget::removeFiles);
//  contextMenu->addSeparator();
  QAction* clear = contextMenu->addAction("Clear");
  connect(clear,&QAction::triggered,this,&FileListWidget::clear);
}

FileListWidget::~FileListWidget()
{
  delete contextMenu;
  delete ui;
}

std::shared_ptr<FileList> FileListWidget::getFileList()
{
  return fileList;
}

void FileListWidget::setFileList(std::shared_ptr<FileList>& list)
{
  fileList = list;
  ui->fileList->setModel(list.get());
}

void FileListWidget::clear()
{
  if (fileList) fileList->clear();
}

void FileListWidget::load()
{
  if (fileList)
  {
    AppSettings settings;
    QString fn = settings.getOpenFilename(this,AppSettings::PATH_FILELIST,"File list (*.lst);;All files (*)");
    if (!fn.isNull())
    {
      fileList->load(fn);
    }
  }
}

void FileListWidget::append()
{
  if (fileList)
  {
    AppSettings settings;
    QString fn = settings.getOpenFilename(this,AppSettings::PATH_FILELIST,"File list (*.lst);;All files (*)");
    if (!fn.isNull())
    {
      fileList->append(fn);
    }
  }
}

void FileListWidget::save()
{
  if (fileList)
  {
    AppSettings settings;
    QString fn = settings.getSaveFilename(this,AppSettings::PATH_FILELIST,"File list (*.lst);;All files (*)");
    if (!fn.isNull())
    {
      fileList->save(fn);
    }
  }
}

int FileListWidget::next()
{
  int n = (ui->fileList->currentIndex().row() + 1) % fileList->rowCount();
  ui->fileList->setCurrentIndex(ui->fileList->model()->index(n,0,QModelIndex()));
  return n;
}

int FileListWidget::previous()
{
  int n = ui->fileList->currentIndex().row() - 1;
  if (n < 0) n += fileList->rowCount();
  ui->fileList->setCurrentIndex(ui->fileList->model()->index(n,0,QModelIndex()));
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




void FileListWidget::contextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->fileList->mapToGlobal(pos));
}

void FileListWidget::doubleClicked(const QModelIndex &index)
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

void FileListWidget::search()
{
  QString txt = QInputDialog::getText(this,"Search","Filename contains:");
  if (!txt.isEmpty())
  {
    QModelIndex index = fileList->find(txt);
    if (index.isValid())
    {
      ui->fileList->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
      ui->fileList->scrollTo(index);
    }
  }
}

void FileListWidget::copySelectionToClipboard()
{
  QString txt = "";
  for (int32_t row=0;row<fileList->rowCount();row++)
  {
    if (ui->fileList->selectionModel()->isRowSelected(row,QModelIndex()))
    {
      txt += fileList->getFiles()[row].absoluteFilePath();
      txt += "\n";
    }
  }
  QApplication::clipboard()->setText(txt);
}

void FileListWidget::copyFiles()
{
  std::vector<QFileInfo> filelist;
  for (int32_t row=0;row<fileList->rowCount();row++)
  {
    if (ui->fileList->selectionModel()->isRowSelected(row,QModelIndex()))
    {
      filelist.push_back(fileList->getFiles()[row]);
    }
  }
  if (!filelist.empty())
  {
    QSettings settings;
    QString path = settings.value(AppSettings::PATH_LAST,QDir::homePath()).toString();
    QString p = QFileDialog::getExistingDirectory(this,"Copy Files",path);
    if (!p.isNull())
    {
      settings.setValue(AppSettings::PATH_LAST,p);
      QDir dir(p);
      for (const QFileInfo& info : filelist)
      {
        bool ret = QFile::copy(info.absoluteFilePath(),dir.absoluteFilePath(info.fileName()));
        if (!ret)
        {
          qWarning() << "Failed to copy " << info.absoluteFilePath() << " to " << dir.absoluteFilePath(info.fileName());
        }
      }
    }
  }
}
