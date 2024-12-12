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

#include "filelist.h"
#include <QFile>
#include <QTextStream>

FileList::FileList(QObject *parent):QAbstractItemModel(parent)
{
}

void FileList::clear()
{
  if (!files.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,files.size()-1);
    files.clear();
    endRemoveRows();
  }
}

void FileList::setFiles(const std::vector<QFileInfo> list)
{
  clear();
  QModelIndex parent;
  beginInsertRows(parent,0,list.size()-1);
  files = list;
  endInsertRows();
}

void FileList::addFile(const QFileInfo &info)
{
  QModelIndex parent;
  beginInsertRows(parent,files.size(),files.size());
  files.push_back(info);
  endInsertRows();
}

void FileList::addFiles(const std::vector<QFileInfo> list)
{
  QModelIndex parent;
  beginInsertRows(parent,files.size(),files.size()+list.size()-1);
  files.insert(files.end(),list.begin(),list.end());
  endInsertRows();
}

const std::vector<QFileInfo>& FileList::getFiles() const
{
  return files;
}


QModelIndex FileList::index(int row, int column, const QModelIndex& /*parent*/) const
{
  return createIndex(row,column);
}

QModelIndex FileList::parent(const QModelIndex& /*index*/) const
{
  return QModelIndex();
}

int FileList::rowCount(const QModelIndex& /*parent*/) const
{
  return static_cast<int>(files.size());
}

int FileList::columnCount(const QModelIndex& /*parent*/) const
{
  return 1;
}

QVariant FileList::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (role == Qt::DisplayRole)
  {
    return QVariant(files[index.row()].fileName());
  }
  return QVariant();
}

bool FileList::save(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::WriteOnly)) return false;
  QTextStream s(&file);
  for (const QFileInfo& info : files)
  {
    s << info.absoluteFilePath() << "\n";
  }
  s.flush();
  file.close();
  return true;
}

bool FileList::load(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly)) return false;
  std::vector<QFileInfo> list;
  QTextStream s(&file);
  QString line = s.readLine();
  while (!line.isNull())
  {
    list.push_back(QFileInfo(line));
    line = s.readLine();
  }
  file.close();
  setFiles(list);
  return true;
}
