/********************************************************************************
 *                                                                              *
 * FitsIP - table model for the profiler table                                  *
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

#ifndef PROFILERTABLEMODEL_H
#define PROFILERTABLEMODEL_H

#include <fitsip/core/profiling/profilerentry.h>
#include <QAbstractTableModel>
#include <vector>

class ProfilerTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit ProfilerTableModel(QObject *parent = nullptr);

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  const std::vector<ProfilerEntry>& getEntries() const;

public:

  void clear();

  void addProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

private:
  std::vector<ProfilerEntry> entries;

  static std::vector<QString> headers;

};

#endif // PROFILERTABLEMODEL_H
