#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H

#include "annotation.h"
#include <QAbstractTableModel>
#include <vector>

class Annotations: public QAbstractTableModel
{
  Q_OBJECT
public:
  Annotations(QObject* parent=nullptr);

  void clear();

  void addAnnotation(const Annotation& a);

  void setAnnotations(const std::vector<Annotation>& list);

  const std::vector<Annotation>& getAnnotations() const;

  void removeAnnotations(const QModelIndexList& list);

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
  std::vector<Annotation> list;

  static std::vector<QString> headers;

};

#endif // ANNOTATIONS_H
