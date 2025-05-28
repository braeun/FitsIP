/********************************************************************************
 *                                                                              *
 * FitsIP - list of selected pixels                                             *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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

#ifndef PIXELLIST_H
#define PIXELLIST_H

#include "pixel.h"
#include <QAbstractTableModel>
#include <vector>

class PixelList: public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit PixelList(QObject* parent=nullptr);

  void clear();

  void addPixel(const Pixel& pixel);

  void setPixels(const std::vector<Pixel>& list);

  const std::vector<Pixel>& getPixels() const;

  void removePixels(const QModelIndexList& list);

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  void shift(int dx, int dy);

  bool save(const QString& filename);

  bool load(const QString& filename);

  PixelList* copy();

private:
  std::vector<Pixel> pixels;

  static std::vector<QString> headers;

};

#endif // PIXELLIST_H
