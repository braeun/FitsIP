/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the filesystem view and associated controls       *
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

#include "filesystemview.h"
#include "ui_filesystemview.h"
#include "appsettings.h"
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <utility>

FileSystemView::FileSystemView(QWidget *parent):QWidget(parent),
  ui(new Ui::FileSystemView)
{
  ui->setupUi(this);
  filesystemModel = new QFileSystemModel();
  filesystemModel->setNameFilterDisables(false);
  QSettings settings;
  QString rootpath = settings.value(AppSettings::PATH_ROOT,QDir::homePath()).toString();
  filesystemModel->setRootPath(rootpath);
  ui->filesystemView->setModel(filesystemModel);
  ui->filesystemView->setRootIndex(filesystemModel->index(rootpath));
  ui->rootField->setText(rootpath);
  ui->filterField->setText(AppSettings().getFileFilter());
  ui->filesystemView->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
  QString path = settings.value(AppSettings::PATH_IMAGE,".").toString();
  ui->filesystemView->setCurrentIndex(filesystemModel->index(path));
  ui->filesystemView->setExpanded(filesystemModel->index(path),true);
  ui->filesystemView->hideColumn(2);
//  ui->filesystemView->hideColumn(3);

  folderContextMenu = new QMenu(this);
  connect(folderContextMenu->addAction("New Folder..."),&QAction::triggered,this,[this]{newFolder();});
  connect(folderContextMenu->addAction("Rename Folder..."),&QAction::triggered,this,[this]{rename();});
  folderContextMenu->addSeparator();
  connect(folderContextMenu->addAction("Remove Folder(s)..."),&QAction::triggered,this,[this]{remove();});

  fileContextMenu = new QMenu(this);
  connect(fileContextMenu->addAction("Open File(s)"),&QAction::triggered,this,[this]{emit openSelection();});
  connect(fileContextMenu->addAction("Copy Filename(s) to List"),&QAction::triggered,this,[this]{emit copySelectionToFilelist();});
  fileContextMenu->addSeparator();
  connect(fileContextMenu->addAction("Rename File..."),&QAction::triggered,this,[this]{rename();});
  fileContextMenu->addSeparator();
  connect(fileContextMenu->addAction("Remove File(s)..."),&QAction::triggered,this,[this]{remove();});

  connect(ui->filesystemView,&QTreeView::doubleClicked,this,[this](const QModelIndex &index){if (QFileInfo(filesystemModel->filePath(index)).isFile()) emit openFile(filesystemModel->filePath(index));});
  connect(ui->filesystemView,&QTreeView::customContextMenuRequested,this,[this](const QPoint& pos){showContextMenu(pos);});
  connect(ui->setRootButton,&QAbstractButton::clicked,this,[this]{selectRoot();});
  connect(ui->rootField,&QLineEdit::returnPressed,this,[this]{rootChanged();});
  connect(ui->openSelectionButton,&QAbstractButton::clicked,this,[this]{emit openSelection();});
  connect(ui->copySelectionToListButton,&QAbstractButton::clicked,this,[this]{emit copySelectionToFilelist();});

  on_filterField_returnPressed();
}

FileSystemView::~FileSystemView()
{
  delete ui;
}

bool FileSystemView::hasSelectedRows() const
{
  return !ui->filesystemView->selectionModel()->selectedRows().isEmpty();
}

QStringList FileSystemView::getSelectedFiles() const
{
  QStringList files;
  QModelIndexList list = ui->filesystemView->selectionModel()->selectedRows();
  for (const QModelIndex& index : std::as_const(list))
  {
    QFileInfo info(filesystemModel->filePath(index));
    if (info.isFile()) files.push_back(info.absoluteFilePath());
  }
  return files;
}


void FileSystemView::selectRoot()
{
  QSettings settings;
  QString rootpath = settings.value(AppSettings::PATH_ROOT,QDir::homePath()).toString();
  QString dir = QFileDialog::getExistingDirectory(this,QApplication::applicationDisplayName(),rootpath);
  if (!dir.isEmpty())
  {
    ui->rootField->setText(dir);
    filesystemModel->setRootPath(dir);
    ui->filesystemView->setRootIndex(filesystemModel->index(dir));
    settings.setValue(AppSettings::PATH_ROOT,dir);
  }
}

void FileSystemView::rootChanged()
{
  QString dir = ui->rootField->text();
  if (!dir.isEmpty() && QDir(dir).exists())
  {
    filesystemModel->setRootPath(dir);
    ui->filesystemView->setRootIndex(filesystemModel->index(dir));
    QSettings settings;
    settings.setValue(AppSettings::PATH_ROOT,dir);
  }
}

void FileSystemView::showContextMenu(const QPoint &pos)
{
  contextIndex = ui->filesystemView->indexAt(pos);
  QFileInfo info(filesystemModel->filePath(contextIndex));
  if (info.isDir())
    folderContextMenu->popup(ui->filesystemView->mapToGlobal(pos));
  else
    fileContextMenu->popup(ui->filesystemView->mapToGlobal(pos));
}

void FileSystemView::newFolder()
{
  QString name = QInputDialog::getText(this,QApplication::applicationDisplayName(),"New folder:");
  if (!name.isEmpty())
  {
    filesystemModel->mkdir(contextIndex,name);
    filesystemModel->sort(0);
  }
}

void FileSystemView::rename()
{
  QString name = QInputDialog::getText(this,QApplication::applicationDisplayName(),"New name:");
  if (!name.isEmpty())
  {
    QFileInfo info(filesystemModel->filePath(contextIndex));
    bool ok = info.absoluteDir().rename(info.fileName(),name);
    if (!ok)
    {
      QString txt = QString("Failed rename the %1 '%2'?").arg(info.isDir()?"folder":"file").arg(info.fileName());
      QMessageBox::warning(this,QApplication::applicationDisplayName(),txt);
    }
  }
}

void FileSystemView::remove()
{
  if (ui->filesystemView->selectionModel()->selectedIndexes().size() > 1)
  {
    if (QMessageBox::question(this,QApplication::applicationDisplayName(),"Really remove the selected file(s)/folder(s)") == QMessageBox::Yes)
    {
      QStringList files;
      QModelIndexList list = ui->filesystemView->selectionModel()->selectedRows();
      for (const QModelIndex& index : list)
      {
        files.push_back(filesystemModel->filePath(index));
      }
      for (const QString& file : files)
      {
        QFileInfo info(file);
        if (info.isDir())
        {
          QDir(info.absoluteFilePath()).removeRecursively();
        }
        else
        {
          QFile(info.absoluteFilePath()).remove();
        }
      }
    }
  }
  else
  {
    QFileInfo info(filesystemModel->filePath(contextIndex));
    QString txt = QString("Really remove the %1 '%2'?").arg(info.isDir()?"folder":"file").arg(info.fileName());
    if (QMessageBox::question(this,QApplication::applicationDisplayName(),txt) == QMessageBox::Yes)
    {
      bool ok = filesystemModel->remove(contextIndex);
      if (!ok)
      {
        txt = QString("Failed remove the %1 '%2'?").arg(info.isDir()?"folder":"file").arg(info.fileName());
        QMessageBox::warning(this,QApplication::applicationDisplayName(),txt);
      }
    }
  }
}

void FileSystemView::on_filterField_returnPressed()
{
  QStringList list;
  if (!ui->filterField->text().isEmpty())
  {
    list.append(ui->filterField->text());
  }
  filesystemModel->setNameFilters(list);
  AppSettings().setFileFilter(ui->filterField->text());
}

