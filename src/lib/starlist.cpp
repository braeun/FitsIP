/********************************************************************************
 *                                                                              *
 * FitsIP - list of selected/detected stars                                     *
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

#include "starlist.h"
#include <cmath>
#include <QFile>
#include <QTextStream>

std::vector<QString> StarList::headers{
  "X", "Y", "FWHM", "X Width", "Y Width", "Roundness", "Sharpness", "Hotness"
};

StarList* StarList::globalStarlist = nullptr;

StarList::StarList(QObject* parent):QAbstractTableModel(parent)
{
}

void StarList::clear()
{
  if (!stars.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,stars.size()-1);
    stars.clear();
    endRemoveRows();
  }
}

void StarList::setStars(const std::vector<Star> &list)
{
  clear();
  QModelIndex parent;
  beginInsertRows(parent,0,list.size()-1);
  stars = list;
  endInsertRows();
}

const std::vector<Star>& StarList::getStars() const
{
  return stars;
}


QVariant StarList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) return QAbstractTableModel::headerData(section,orientation,role);
  if (orientation == Qt::Orientation::Horizontal)
  {
    return headers[section];
  }
  return QVariant(section);
}

int StarList::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return stars.size();
}

int StarList::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return headers.size();
}


QVariant StarList::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.row() < 0) return QVariant();
  const Star& star = stars[index.row()];
  switch (index.column())
  {
    case 0:
      return QVariant(star.x);
    case 1:
      return QVariant(star.y);
    case 2:
      return QVariant(star.fwhm);
    case 3:
      return QVariant(star.xwidth);
    case 4:
      return QVariant(star.ywidth);
    case 5:
      return QVariant(star.round);
    case 6:
      return QVariant(star.sharp);
    case 7:
      return QVariant(star.hotness);
  }
  return QVariant();
}

void StarList::shift(double dx, double dy)
{
  emit layoutAboutToBeChanged();
  for (Star& star : stars)
  {
    star.x += dx;
    star.y += dy;
  }
  emit layoutChanged();
}

void StarList::rotate(double xc, double yc, double a)
{
  emit layoutAboutToBeChanged();
  a *= M_PI / 180.0;
  double ca = cos(a);
  double sa = sin(a);
  for (Star& star : stars)
  {
    double xr = (star.x - xc) * ca - (star.y - yc) * sa;
    double yr = (star.x - xc) * sa + (star.y - yc) * ca;
    star.x = xr + xc;
    star.y = yr + yc;
  }
  emit layoutChanged();
}

bool StarList::save(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::WriteOnly|QFile::Text)) return false;
  QTextStream s(&file);
  for (const Star& star : stars)
  {
    s << star.x << "," << star.y << "," << star.xwidth << "," << star.ywidth;
    s << "," << star.fwhm << "," << star.round << "," << star.sharp << "," << star.hotness;
    s << "\n";
  }
  s.flush();
  file.close();
  return true;
}

bool StarList::load(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly|QFile::Text)) return false;
  std::vector<Star> list;
  QTextStream s(&file);
  QString line = s.readLine();
  while (!line.isNull())
  {
    QStringList tokens = line.split(",");
    if (tokens.size() >= 8)
    {
      Star star;
      star.x = tokens[0].toDouble();
      star.y = tokens[1].toDouble();
      star.xwidth = tokens[2].toDouble();
      star.ywidth = tokens[3].toDouble();
      star.fwhm = tokens[4].toDouble();
      star.round = tokens[5].toDouble();
      star.sharp = tokens[6].toDouble();
      star.hotness = tokens[7].toDouble();
      list.push_back(star);
    }
    line = s.readLine();
  }
  file.close();
  setStars(list);
  return true;
}

StarList* StarList::getGlobalInstance()
{
  if (!globalStarlist) globalStarlist = new StarList;
  return globalStarlist;
}
