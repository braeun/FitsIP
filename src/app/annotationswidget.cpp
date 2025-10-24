#include "annotationswidget.h"
#include <fitsip/core/annotations.h>
#include <QTableView>
#include <QVBoxLayout>

AnnotationsWidget::AnnotationsWidget(QWidget *parent):QWidget{parent},
  annotations(nullptr)
{
  setObjectName(QString::fromUtf8("AnnotationWidget"));
  resize(400, 300);
  QVBoxLayout* verticalLayout = new QVBoxLayout(this);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  table = new QTableView(this);
  table->setObjectName(QString::fromUtf8("annotationsTable"));
  table->setContextMenuPolicy(Qt::CustomContextMenu);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  verticalLayout->addWidget(table);
}

void AnnotationsWidget::setAnnotations(Annotations* list)
{
  annotations = list;
  QItemSelectionModel *m = table->selectionModel();
  table->setModel(list);
  delete m;

}

void AnnotationsWidget::clear()
{
  annotations->clear();
}




