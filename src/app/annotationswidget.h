#ifndef ANNOTATIONSWIDGET_H
#define ANNOTATIONSWIDGET_H

#include <QWidget>

class Annotations;
class QTableView;

class AnnotationsWidget : public QWidget
{
  Q_OBJECT
public:
  explicit AnnotationsWidget(QWidget *parent = nullptr);

  void setAnnotations(Annotations* list);

  void clear();

private:
  QTableView* table;
  Annotations* annotations;
};

#endif // ANNOTATIONSWIDGET_H
