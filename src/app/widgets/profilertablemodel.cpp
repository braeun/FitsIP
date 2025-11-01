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

#include "profilertablemodel.h"


std::vector<QString> ProfilerTableModel::headers{
  "Profiler", "Image", "Size", "Time", "Notes"
};

ProfilerTableModel::ProfilerTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

QVariant ProfilerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) return QAbstractTableModel::headerData(section,orientation,role);
  if (orientation == Qt::Orientation::Horizontal)
  {
    return headers[section];
  }
  return QVariant(section);
}

int ProfilerTableModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return entries.size();
}

int ProfilerTableModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return headers.size();
}

QVariant ProfilerTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.row() < 0) return QVariant();
  switch (index.column())
  {
    case 0:
      return QVariant(entries[index.row()].profiler);
    case 1:
      return QVariant(entries[index.row()].image);
    case 2:
      return QVariant(QString::asprintf("%d x %d",entries[index.row()].w,entries[index.row()].h));
    case 3:
      return QVariant(QString::asprintf("%.1f ms",entries[index.row()].t/1000.0));
    case 4:
      return QVariant(entries[index.row()].notes);
  }
  return QVariant();
}

const std::vector<ProfilerEntry>& ProfilerTableModel::getEntries() const
{
  return entries;
}


void ProfilerTableModel::clear()
{
  emit layoutAboutToBeChanged();
  entries.clear();
  emit layoutChanged();
}

void ProfilerTableModel::addProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes)
{
  ProfilerEntry entry;
  entry.profiler = profiler;
  entry.image = image;
  entry.w = w;
  entry.h = h;
  entry.t = t;
  entry.notes = notes;
  emit layoutAboutToBeChanged();
  entries.push_back(entry);
  emit layoutChanged();
}
