#include "historytablewidget.h"
#include <fitsip/core/imagemetadata.h>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>

HistoryTableWidget::HistoryTableWidget(QWidget *parent):QWidget{parent}
{
  setObjectName("HistoryTableWidget");
  verticalLayout = new QVBoxLayout(this);
  tableWidget = new QTableWidget(this);
  tableWidget->setColumnCount(1);
  tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
  tableWidget->setRowCount(6);
  tableWidget->setColumnCount(1);
  tableWidget->horizontalHeader()->setVisible(false);
  tableWidget->horizontalHeader()->setStretchLastSection(true);
  verticalLayout->addWidget(tableWidget);
}

void HistoryTableWidget::clear()
{
  tableWidget->clearContents();
  tableWidget->setRowCount(0);
}

void HistoryTableWidget::setData(const ImageMetadata& data)
{
  int first = tableWidget->rowCount();
  tableWidget->setRowCount(data.getHistory().size());
  for (int i=first;i<data.getHistory().size();i++)
  {
    tableWidget->setItem(i,0,new QTableWidgetItem(data.getHistory()[i]));
  }
}

// void HistoryTableWidget::setFile(std::shared_ptr<FitsObject> file)
// {
//   if (file)
//   {
//     const ImageMetadata& metadata = file->getImage().getMetadata();
//     int first = tableWidget->rowCount();
//     tableWidget->setRowCount(metadata.getHistory().size());
//     for (int i=first;i<metadata.getHistory().size();i++)
//     {
//       tableWidget->setItem(i,0,new QTableWidgetItem(metadata.getHistory()[i]));
//     }
//   }
//   else
//   {
//     clear();
//   }
// }
