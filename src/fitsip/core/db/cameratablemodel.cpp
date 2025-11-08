#include "cameratablemodel.h"
#include "database.h"
#include <QDebug>
#include <QMessageBox>

std::vector<QString> CameraTableModel::headers{
  "Name", "Pixel Width [µm]", "Pixel Height [µm]"
};

CameraTableModel::CameraTableModel(QObject *parent):QAbstractTableModel(parent),
  database("")
{
  refresh();
}

QVariant CameraTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole) return QAbstractTableModel::headerData(section,orientation,role);
  if (orientation == Qt::Orientation::Horizontal)
  {
    return headers[section];
  }
  return QVariant(section);
}

int CameraTableModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) return 0;
  return cameras.size();
}

int CameraTableModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) return 0;
  return headers.size();
}

QVariant CameraTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.row() < 0) return QVariant();
  switch (index.column())
  {
    case 0:
      return QVariant(cameras[index.row()].getName());
    case 1:
      return QVariant(cameras[index.row()].getPixelwidth());
    case 2:
      return QVariant(cameras[index.row()].getPixelheight());
  }
  return QVariant();
}

bool CameraTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid()) return false;
  if (role != Qt::DisplayRole) return false;
  if (index.row() < 0) return false;
  Camera& camera = cameras[index.row()];
  switch (index.column())
  {
    case 1:
      camera.setPixelwidth(value.toDouble());
      break;
    case 2:
      camera.setPixelheight(value.toDouble());
      break;
  }
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Camera Database","Failed to connect to database!");
  }
  else
  {
    db->updateCamera(camera);
  }
  emit dataChanged(index,index);
  return true;
}

Qt::ItemFlags CameraTableModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  if (index.column() == 1 || index.column() == 2) flags |= Qt::ItemIsEditable;
  return flags;
}

void CameraTableModel::clear()
{
  if (!cameras.empty())
  {
    QModelIndex parent;
    beginRemoveRows(parent,0,cameras.size()-1);
    cameras.clear();
    endRemoveRows();
  }
}

void CameraTableModel::refresh()
{
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Camera Database","Failed to connect to database!");
  }
  else
  {
    clear();
    cameras = db->getCameras();
    QModelIndex parent;
    beginInsertRows(parent,0,cameras.size()-1);
    endInsertRows();
  }
}

void CameraTableModel::remove(QModelIndexList list)
{
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Camera Database","Failed to connect to database!");
  }
  else
  {
    for (int i=0;i<list.size();++i)
    {
      int row = list[i].row();
      db->removeCamera(cameras[row].getName());
    }
    refresh();
  }
}

void CameraTableModel::addCamera(const Camera& camera)
{
  Database* db = Database::get(database);
  if (!db)
  {
    QMessageBox::warning(nullptr,"Camera Database","Failed to connect to database!");
  }
  else
  {
    db->addCamera(camera);
    refresh();
  }
}
