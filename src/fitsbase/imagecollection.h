/********************************************************************************
 *                                                                              *
 * FitsIP - collection of fits objects, i.e. opened images                      *
 *                                                                              *
 * modified: 2025-01-30                                                         *
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

#ifndef IMAGECOLLECTION_H
#define IMAGECOLLECTION_H

#include "fitsobject.h"
#include <QAbstractItemModel>
#include <vector>
#include <memory>

/**
 * @brief Collection of loaded images.
 */
class ImageCollection: public QAbstractItemModel
{
  Q_OBJECT

public:
  ImageCollection();

  bool isEmpty() const;

  size_t size() const;

  void setActiveFile(std::shared_ptr<FitsObject> file);

  std::shared_ptr<FitsObject> setActiveFile(int index);

  inline std::shared_ptr<FitsObject> getActiveFile() const;

  std::shared_ptr<FitsObject> getFile(int id) const;

  std::shared_ptr<FitsObject> getFile(const QString& filename) const;

  void removeActiveFile();

  void removeAll();

  void addFile(std::shared_ptr<FitsObject> file);

  inline const std::vector<std::shared_ptr<FitsObject>>& getFiles() const;

//  inline static ImageCollection& getGlobal();

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &child) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
  std::shared_ptr<FitsObject> activeFile;
  std::vector<std::shared_ptr<FitsObject>> files;

  static ImageCollection globalCollection;
};

inline std::shared_ptr<FitsObject> ImageCollection::getActiveFile() const
{
  return activeFile;
}

inline const std::vector<std::shared_ptr<FitsObject>>& ImageCollection::getFiles() const
{
  return files;
}

// inline ImageCollection& ImageCollection::getGlobal()
// {
//   return globalCollection;
// }

#endif // IMAGECOLLECTION_H
