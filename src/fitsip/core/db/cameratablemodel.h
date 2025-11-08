#ifndef CAMERATABLEMODEL_H
#define CAMERATABLEMODEL_H

#include "camera.h"
#include <QAbstractTableModel>
#include <vector>

class CameraTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit CameraTableModel(QObject *parent = nullptr);

  // Header:
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  // Basic functionality:
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  void refresh();

  void remove(QModelIndexList list);

  void addCamera(const Camera& camera);

private:
  void clear();

  QString database;
  std::vector<Camera> cameras;

  static std::vector<QString> headers;

};

#endif // CAMERATABLEMODEL_H
