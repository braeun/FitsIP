/********************************************************************************
 *                                                                              *
 * FitsIP - general purpose file list                                          *
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

#ifndef FILELIST_H
#define FILELIST_H

#include <QAbstractItemModel>
#include <QFileInfo>
#include <vector>

class FileList: public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit FileList(QObject *parent = nullptr);

  void clear();

  void setFiles(const std::vector<QFileInfo> list);

  void addFile(const QFileInfo& info);

  void addFiles(const std::vector<QFileInfo> list);

  const std::vector<QFileInfo>& getFiles() const;

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &child) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  bool save(const QString& filename);

  bool load(const QString& filename);

private:
  std::vector<QFileInfo> files;
};

#endif // FILELIST_H
