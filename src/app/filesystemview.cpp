/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the filesystem view and associated controls       *
 *                                                                              *
 * modified: 2025-10-24                                                         *
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
#include <fitsip/core/io/iofactory.h>
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
  filterList = AppSettings().getFileFilters();
  if (filterList.isEmpty())
  {
    filterList.push_back("*.fts*");
  }
  filesystemModel = new QFileSystemModel();
  filesystemModel->setNameFilterDisables(false);
  QSettings settings;
  QString rootpath = settings.value(AppSettings::PATH_ROOT,QDir::homePath()).toString();
  filesystemModel->setRootPath(rootpath);
  ui->filesystemView->setModel(filesystemModel);
  ui->filesystemView->setRootIndex(filesystemModel->index(rootpath));
  ui->rootField->setText(rootpath);
  ui->filterBox->addItems(filterList);
  ui->filesystemView->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
  QString path = settings.value(AppSettings::PATH_IMAGE,".").toString();
  ui->filesystemView->setCurrentIndex(filesystemModel->index(path));
  ui->filesystemView->setExpanded(filesystemModel->index(path),true);
  ui->filesystemView->hideColumn(2);
//  ui->filesystemView->hideColumn(3);

  folderContextMenu = new QMenu(this);
  connect(folderContextMenu->addAction("Set as Root"),&QAction::triggered,this,[this]{selectedAsRoot();});
  folderContextMenu->addSeparator();
  connect(folderContextMenu->addAction("New Folder..."),&QAction::triggered,this,[this]{newFolder();});
  connect(folderContextMenu->addAction("Rename Folder..."),&QAction::triggered,this,[this]{rename();});
  folderContextMenu->addSeparator();
  connect(folderContextMenu->addAction("Remove Folder(s)..."),&QAction::triggered,this,[this]{remove();});
  folderContextMenu->addSeparator();
  connect(folderContextMenu->addAction("Open in Filemanager"),&QAction::triggered,this,[this]{emit openFile(filesystemModel->filePath(contextIndex));});

  imageContextMenu = new QMenu(this);
  connect(imageContextMenu->addAction("Open Image(s)"),&QAction::triggered,this,[this]{emit openSelection();});
  connect(imageContextMenu->addAction("Copy Filename(s) to List"),&QAction::triggered,this,[this]{emit copySelectionToFilelist();});
  imageContextMenu->addSeparator();
  connect(imageContextMenu->addAction("Rename File..."),&QAction::triggered,this,[this]{rename();});
  imageContextMenu->addSeparator();
  connect(imageContextMenu->addAction("Remove File(s)..."),&QAction::triggered,this,[this]{remove();});

  scriptContextMenu = new QMenu(this);
  connect(scriptContextMenu->addAction("Execute Script"),&QAction::triggered,this,[this]{if (QFileInfo(filesystemModel->filePath(contextIndex)).isFile()) emit runFile(filesystemModel->filePath(contextIndex));});
  connect(scriptContextMenu->addAction("Open File"),&QAction::triggered,this,[this]{if (QFileInfo(filesystemModel->filePath(contextIndex)).isFile()) emit openFile(filesystemModel->filePath(contextIndex));});
  scriptContextMenu->addSeparator();
  connect(scriptContextMenu->addAction("Rename File..."),&QAction::triggered,this,[this]{rename();});
  scriptContextMenu->addSeparator();
  connect(scriptContextMenu->addAction("Remove File(s)..."),&QAction::triggered,this,[this]{remove();});

  fileContextMenu = new QMenu(this);
  connect(fileContextMenu->addAction("Rename File..."),&QAction::triggered,this,[this]{rename();});
  fileContextMenu->addSeparator();
  connect(fileContextMenu->addAction("Remove File(s)..."),&QAction::triggered,this,[this]{remove();});

  connect(ui->filesystemView,&QTreeView::doubleClicked,this,&FileSystemView::handleDoubleClick);
  connect(ui->filesystemView,&QTreeView::customContextMenuRequested,this,[this](const QPoint& pos){showContextMenu(pos);});
  connect(ui->setRootButton,&QAbstractButton::clicked,this,[this]{selectRoot();});
  connect(ui->rootField,&QLineEdit::returnPressed,this,[this]{rootChanged();});
  connect(ui->openSelectionButton,&QAbstractButton::clicked,this,[this]{emit openSelection();});
  connect(ui->copySelectionToListButton,&QAbstractButton::clicked,this,[this]{emit copySelectionToFilelist();});
  connect(ui->clearFilterButton,&QAbstractButton::clicked,this,[this]{clearFilterlist();});

  connect(ui->filterBox->lineEdit(),&QLineEdit::editingFinished,this,[this](){filterChanged(ui->filterBox->currentText());});
  connect(ui->filterBox,qOverload<int>(&QComboBox::currentIndexChanged),this,[this](int i){filterChanged(ui->filterBox->currentText());});

  ui->filterBox->setCurrentText(AppSettings().getCurrentFileFilter());
  filterChanged(AppSettings().getCurrentFileFilter());
//  if (!filterList.isEmpty())
//  {
//    ui->filterBox->setCurrentIndex(0);
//    filterChanged(ui->filterBox->currentText());
//  }
}

FileSystemView::~FileSystemView()
{
  delete ui;
}

QString FileSystemView::getRoot() const
{
  QSettings settings;
  return settings.value(AppSettings::PATH_ROOT,ui->rootField->text()).toString();
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
  setRoot(dir);
}

void FileSystemView::rootChanged()
{
  setRoot(ui->rootField->text());
}

void FileSystemView::selectedAsRoot()
{
  QFileInfo info(filesystemModel->filePath(contextIndex));
  if (info.isDir()) setRoot(info.absoluteFilePath());
}

void FileSystemView::setRoot(const QString &dir)
{
  if (!dir.isEmpty() && QDir(dir).exists())
  {
    ui->rootField->setText(dir);
    filesystemModel->setRootPath(dir);
    ui->filesystemView->setRootIndex(filesystemModel->index(dir));
    QSettings settings;
    settings.setValue(AppSettings::PATH_ROOT,dir);
    emit workingDirChanged(dir);
  }
}

void FileSystemView::showContextMenu(const QPoint &pos)
{
  contextIndex = ui->filesystemView->indexAt(pos);
  QFileInfo info(filesystemModel->filePath(contextIndex));
  if (info.isDir())
    folderContextMenu->popup(ui->filesystemView->mapToGlobal(pos));
  else
  {
    if (IOFactory::getInstance()->isImage(info.fileName()))
      imageContextMenu->popup(ui->filesystemView->mapToGlobal(pos));
    else if (IOFactory::getInstance()->isScript(info.fileName()))
      scriptContextMenu->popup(ui->filesystemView->mapToGlobal(pos));
    else
      fileContextMenu->popup(ui->filesystemView->mapToGlobal(pos));
  }
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
  QFileInfo info(filesystemModel->filePath(contextIndex));
  QString name = QInputDialog::getText(this,QApplication::applicationDisplayName(),"New name:",QLineEdit::Normal,info.fileName());
  if (!name.isEmpty())
  {
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

void FileSystemView::handleDoubleClick(const QModelIndex &index)
{
  QFileInfo info(filesystemModel->filePath(index));
  if (info.isFile())
  {
    if (IOFactory::getInstance()->isScript(info.fileName()))
      emit runFile(info.absoluteFilePath());
    else
      emit openFile(info.absoluteFilePath());
  }
}


void FileSystemView::filterChanged(const QString& text)
{
  QStringList list;
  if (!text.isEmpty())
  {
    list.append(text);
  }
  AppSettings().setCurrentFileFilter(text);
  filesystemModel->setNameFilters(list);
  if (!text.isEmpty())
  {
    if (!filterList.contains(text))
    {
      filterList.push_back(text);
      AppSettings().setFileFilters(filterList);
    }
  }
}

void FileSystemView::clearFilterlist()
{
  filterList.clear();
  AppSettings().setFileFilters(filterList);
  filesystemModel->setNameFilters(QStringList());
  ui->filterBox->clear();
}
