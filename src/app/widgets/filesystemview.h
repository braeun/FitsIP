/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the filesystem view and associated controls       *
 *                                                                              *
 * modified: 2025-01-28                                                         *
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

#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QModelIndex>
#include <QStringList>
#include <QWidget>

class QFileSystemModel;
class QMenu;

namespace Ui {
class FileSystemView;
}

class FileSystemView : public QWidget
{
  Q_OBJECT

public:
  explicit FileSystemView(QWidget *parent = nullptr);
  ~FileSystemView();

  QString getRoot() const;

  void setRoot(const QString& dir);

  bool hasSelectedRows() const;

  /**
   * @brief Return a list of selected files.
   *
   * This method only returns names of real files. It does not include
   * directories.
   * @return list of selected files.
   */
  QStringList getSelectedFiles() const;

signals:
  void openFile(QString file);
  void runFile(QString file);
  void openSelection();
  void copySelectionToFilelist();
  void workingDirChanged(const QString& dir);

private:
  void selectRoot();
  void rootChanged();
  void selectedAsRoot();
  void showContextMenu(const QPoint& pos);
  void newFolder();
  void rename();
  void remove();
  void handleDoubleClick(const QModelIndex &index);
  void filterChanged(const QString& text);
  void clearFilterlist();

  Ui::FileSystemView *ui;
  QFileSystemModel* filesystemModel;
  QMenu* folderContextMenu;
  QMenu* imageContextMenu;
  QMenu* scriptContextMenu;
  QMenu* fileContextMenu;
  QModelIndex contextIndex;
  QStringList filterList;
};

#endif // FILESYSTEMVIEW_H
