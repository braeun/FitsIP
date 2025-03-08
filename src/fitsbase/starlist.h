/********************************************************************************
 *                                                                              *
 * FitsIP - list of selected/detected stars                                     *
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

#ifndef STARLIST_H
#define STARLIST_H

#include "star.h"
#include <QAbstractTableModel>
#include <vector>

class StarList: public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit StarList(QObject* parent=nullptr);

  void clear();

  void setStars(const std::vector<Star>& list);

  const std::vector<Star>& getStars() const;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  void shift(double dx, double dy);

  void rotate(double xc, double yc, double a);

  bool save(const QString& filename);

  bool load(const QString& filename);

private:
  std::vector<Star> stars;

  static std::vector<QString> headers;

};

#endif // STARLIST_H
