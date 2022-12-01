/********************************************************************************
 *                                                                              *
 * FitsIP - collection of file objects, i.e. opened images                      *
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

#include "imagecollection.h"
#include <QFileInfo>

ImageCollection ImageCollection::globalCollection;

ImageCollection::ImageCollection()
{
}

void ImageCollection::setActiveFile(std::shared_ptr<FileObject> file)
{
  activeFile = file;
}

std::shared_ptr<FileObject> ImageCollection::setActiveFile(int index)
{
  activeFile = files[index];
  return activeFile;
}

std::shared_ptr<FileObject> ImageCollection::getFile(int32_t id) const
{
  for (auto file : files)
  {
    if (file->getId() == id) return file;
  }
  return std::shared_ptr<FileObject>();
}

std::shared_ptr<FileObject> ImageCollection::getFile(const QString& filename) const
{
  for (auto file : files)
  {
    if (file->getFilename() == filename) return file;
  }
  return std::shared_ptr<FileObject>();
}

void ImageCollection::removeActiveFile()
{
  if (!activeFile) return;
  for (uint32_t i=0;i<files.size();i++)
  {
    if (files[i].get() == activeFile.get())
    {
      QModelIndex parent;
      beginRemoveRows(parent,i,i);
      files.erase(files.begin()+i);
      endRemoveRows();
      activeFile.reset();
      break;
    }
  }
}

void ImageCollection::removeAll()
{
  if (!files.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,files.size()-1);
    files.clear();
    endRemoveRows();
  }
}

void ImageCollection::addFile(std::shared_ptr<FileObject> file)
{
  QModelIndex parent;
  beginInsertRows(parent,files.size(),files.size());
  files.push_back(file);
  endInsertRows();
}

QModelIndex ImageCollection::index(int row, int column, const QModelIndex& /*parent*/) const
{
  return createIndex(row,column);
}

QModelIndex ImageCollection::parent(const QModelIndex& /*child*/) const
{
  return QModelIndex();
}

int ImageCollection::rowCount(const QModelIndex& /*parent*/) const
{
  return static_cast<int>(files.size());
}

int ImageCollection::columnCount(const QModelIndex& /*parent*/) const
{
  return 1;
}

QVariant ImageCollection::data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole)
  {
    auto file = files[index.row()];
    if (file->getFilename().isEmpty())
    {
      if (file->getImage()->getName().isEmpty())
      {
        return QVariant(QString("image %1").arg(files[index.row()]->getId()));
      }
      return file->getImage()->getName();
    }
    else
    {
      QFileInfo info(file->getFilename());
      return QVariant(info.fileName());
    }
  }
  return QVariant();
}

