#ifndef TELESCOPETABLEMODEL_H
#define TELESCOPETABLEMODEL_H

#include "telescope.h"
#include <QAbstractTableModel>
#include <vector>

class TelescopeTableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit TelescopeTableModel(QObject *parent = nullptr);

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

  void addTelescope(const Telescope& telescope);

private:
  void clear();

  QString database;
  std::vector<Telescope> telescopes;

  static std::vector<QString> headers;
};

#endif // TELESCOPETABLEMODEL_H
