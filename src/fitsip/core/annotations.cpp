#include "annotations.h"

std::vector<QString> Annotations::headers{
    "XP", "YP", "X", "Y", "Text"
};

Annotations::Annotations(QObject* parent):QAbstractTableModel(parent)
{
}

void Annotations::clear()
{
  if (!list.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,list.size()-1);
    list.clear();
    endRemoveRows();
  }
}

void Annotations::addAnnotation(const Annotation &a)
{
  QModelIndex parent;
  beginInsertRows(parent,list.size(),list.size());
  list.push_back(a);
  endInsertRows();
}

void Annotations::setAnnotations(const std::vector<Annotation> &l)
{
  clear();
  QModelIndex parent;
  beginInsertRows(parent,0,l.size()-1);
  list = l;
  endInsertRows();
}

const std::vector<Annotation>& Annotations::getAnnotations() const
{
  return list;
}

void Annotations::removeAnnotations(const QModelIndexList &l)
{
  int shift = 0;
  emit layoutAboutToBeChanged();
  for (QModelIndex index : l)
  {
    int row = index.row() - shift;
    list.erase(list.begin()+row);
    ++shift;
  }
  emit layoutChanged();
}

QVariant Annotations::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) return QAbstractTableModel::headerData(section,orientation,role);
  if (orientation == Qt::Orientation::Horizontal)
  {
    return headers[section];
  }
  return QVariant(section);
}

int Annotations::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return list.size();
}

int Annotations::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return headers.size();
}


QVariant Annotations::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.row() < 0) return QVariant();
  const Annotation& a = list[index.row()];
  switch (index.column())
  {
    case 0:
      return QVariant(a.getXp());
    case 1:
      return QVariant(a.getYp());
    case 2:
      return QVariant(a.getX());
    case 3:
      return QVariant(a.getY());
    case 4:
      return QVariant(a.getText());
  }
  return QVariant();
}

