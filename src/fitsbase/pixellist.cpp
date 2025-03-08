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

#include "pixellist.h"
#include <QFile>
#include <QTextStream>

std::vector<QString> PixelList::headers{
  "X", "Y", "Value 1", "Value 2", "Value 3"
};

PixelList::PixelList(QObject* parent):QAbstractTableModel(parent)
{
}

void PixelList::clear()
{
  if (!pixels.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,pixels.size()-1);
    pixels.clear();
    endRemoveRows();
  }
}

void PixelList::addPixel(const Pixel &pixel)
{
  QModelIndex parent;
  beginInsertRows(parent,pixels.size(),pixels.size());
  pixels.push_back(pixel);
  endInsertRows();
}

void PixelList::setPixels(const std::vector<Pixel> &list)
{
  clear();
  QModelIndex parent;
  beginInsertRows(parent,0,list.size()-1);
  pixels = list;
  endInsertRows();
}

const std::vector<Pixel>& PixelList::getPixels() const
{
  return pixels;
}

void PixelList::removePixels(const QModelIndexList &list)
{
  int shift = 0;
  emit layoutAboutToBeChanged();
  for (QModelIndex index : list)
  {
    int row = index.row() - shift;
    pixels.erase(pixels.begin()+row);
    ++shift;
  }
  emit layoutChanged();
}

QVariant PixelList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) return QAbstractTableModel::headerData(section,orientation,role);
  if (orientation == Qt::Orientation::Horizontal)
  {
    return headers[section];
  }
  return QVariant(section);
}

int PixelList::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return pixels.size();
}

int PixelList::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return headers.size();
}


QVariant PixelList::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.row() < 0) return QVariant();
  const Pixel& pixel = pixels[index.row()];
  switch (index.column())
  {
    case 0:
      return QVariant(pixel.x);
    case 1:
      return QVariant(pixel.y);
    case 2:
      return !pixel.i.empty() ? QVariant(pixel.i[0]) : QVariant();
    case 3:
      return pixel.i.size() > 1 ? QVariant(pixel.i[1]) : QVariant();
    case 4:
      return pixel.i.size() > 2 ? QVariant(pixel.i[2]) : QVariant();
  }
  return QVariant();
}

void PixelList::shift(int dx, int dy)
{
  for (Pixel& pixel : pixels)
  {
    pixel.x += dx;
    pixel.y += dy;
  }
}

bool PixelList::save(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::WriteOnly|QFile::Text)) return false;
  QTextStream s(&file);
  for (const Pixel& pixel : pixels)
  {
    s << pixel.x << "," << pixel.y;
    for (ValueType i : pixel.i) s << "," << i;
    s << "\n";
  }
  s.flush();
  file.close();
  return true;
}

bool PixelList::load(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly|QFile::Text)) return false;
  std::vector<Pixel> list;
  QTextStream s(&file);
  QString line = s.readLine();
  while (!line.isNull())
  {
    QStringList tokens = line.split(",");
    if (tokens.size() >= 3)
    {
      Pixel px;
      px.x = tokens[0].toDouble();
      px.y = tokens[1].toDouble();
      for (int i=2;i<tokens.size();i++)
      {
        px.i.push_back(tokens[i].toDouble());
      }
      list.push_back(px);
    }
//    list.push_back(QFileInfo(line));
    line = s.readLine();
  }
  file.close();
  setPixels(list);
  return true;
}

PixelList* PixelList::copy()
{
  PixelList* list = new PixelList;
  list->setPixels(pixels);
  return list;
}

