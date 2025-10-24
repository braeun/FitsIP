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

#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QWidget>
#include <memory>

class FileList;
class QFileInfo;
class QMenu;

namespace Ui {
class FileListWidget;
}

class FileListWidget : public QWidget
{
  Q_OBJECT
public:
  explicit FileListWidget(QWidget *parent = nullptr);
  ~FileListWidget();

  void setWorkingDir(const QString& dir);

  FileList* getFileList();

  void setFileList(FileList* list);

  void clear();

  void save();

  void load();

  void append();

  int next();

  int previous();

  std::vector<QFileInfo> getSelection() const;

signals:
  void openSelected();

private:
  void contextMenuRequested(const QPoint &pos);
  void doubleClicked(const QModelIndex &index);
  void removeFiles();
  void search();
  void copySelectionToClipboard();
  void copyFiles();
  void relocate();

  Ui::FileListWidget *ui;
  QString workingDir;
  FileList* fileList;
  QMenu* contextMenu;
};

#endif // FILELISTWIDGET_H
