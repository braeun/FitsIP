#include "telescopetablemodel.h"
#include "database.h"
#include <QDebug>
#include <QMessageBox>

std::vector<QString> TelescopeTableModel::headers{
  "Name", "f [mm]", "D [mm]"
};

TelescopeTableModel::TelescopeTableModel(QObject *parent):QAbstractTableModel(parent),
  database("")
{
  refresh();
}

QVariant TelescopeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) return QAbstractTableModel::headerData(section,orientation,role);
  if (orientation == Qt::Orientation::Horizontal)
  {
    return headers[section];
  }
  return QVariant(section);
}

int TelescopeTableModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) return 0;
  return telescopes.size();
}

int TelescopeTableModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) return 0;
  return headers.size();
}

QVariant TelescopeTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.row() < 0) return QVariant();
  switch (index.column())
  {
    case 0:
      return QVariant(telescopes[index.row()].getName());
    case 1:
      return QVariant(telescopes[index.row()].getF());
    case 2:
      return QVariant(telescopes[index.row()].getD());
  }
  return QVariant();
}

bool TelescopeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid()) return false;
  if (role != Qt::DisplayRole) return false;
  if (index.row() < 0) return false;
  Telescope& telescope = telescopes[index.row()];
  switch (index.column())
  {
    case 1:
      telescope.setF(value.toDouble());
      break;
    case 2:
      telescope.setD(value.toDouble());
      break;
  }
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Telescope Database","Failed to connect to database!");
  }
  else
  {
    db->updateTelescope(telescope);
  }
  emit dataChanged(index,index);
  return true;
}

Qt::ItemFlags TelescopeTableModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  if (index.column() == 1 || index.column() == 2) flags |= Qt::ItemIsEditable;
  return flags;
}

void TelescopeTableModel::clear()
{
  if (!telescopes.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,telescopes.size()-1);
    telescopes.clear();
    endRemoveRows();
  }
}

void TelescopeTableModel::refresh()
{
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Telescope Database","Failed to connect to database!");
  }
  else
  {
    clear();
    telescopes = db->getTelescopes();
    QModelIndex parent;
    beginInsertRows(parent,0,telescopes.size()-1);
    endInsertRows();
  }
}

void TelescopeTableModel::remove(QModelIndexList list)
{
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Telescope Database","Failed to connect to database!");
  }
  else
  {
    for (int i=0;i<list.size();++i)
    {
      int row = list[i].row();
      db->removeCamera(telescopes[row].getName());
    }
    refresh();
  }
}

void TelescopeTableModel::addTelescope(const Telescope& telescope)
{
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Telescope Database","Failed to connect to database!");
  }
  else
  {
    db->addTelescope(telescope);
    refresh();
  }
}

