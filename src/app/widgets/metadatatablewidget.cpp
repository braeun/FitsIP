#include "metadatatablewidget.h"
#include "ui_metadatatablewidget.h"
#include <fitsip/core/imagemetadata.h>

MetadataTableWidget::MetadataTableWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MetadataTableWidget)
{
  ui->setupUi(this);
}

MetadataTableWidget::~MetadataTableWidget()
{
  delete ui;
}

void MetadataTableWidget::clear()
{
  ui->tableWidget->clearContents();
}

void MetadataTableWidget::setData(const ImageMetadata& data)
{
  ui->tableWidget->setItem(0,0,new QTableWidgetItem(data.getObject()));
  ui->tableWidget->setItem(1,0,new QTableWidgetItem(data.getObsDateTime().toString(Qt::ISODate)));
  ui->tableWidget->setItem(2,0,new QTableWidgetItem(data.getObserver()));
  ui->tableWidget->setItem(3,0,new QTableWidgetItem(data.getTelescope()));
  ui->tableWidget->setItem(4,0,new QTableWidgetItem(data.getInstrument()));
  ui->tableWidget->setItem(5,0,new QTableWidgetItem(QString::number(data.getExposureTime())));
}

// void MetadataTableWidget::setFile(std::shared_ptr<FitsObject> file)
// {
//   if (file)
//   {
//     const ImageMetadata& metadata = file->getImage().getMetadata();
//     ui->tableWidget->setItem(0,0,new QTableWidgetItem(metadata.getObject()));
//     ui->tableWidget->setItem(1,0,new QTableWidgetItem(metadata.getObsDateTime().toString(Qt::ISODate)));
//     ui->tableWidget->setItem(2,0,new QTableWidgetItem(metadata.getObserver()));
//     ui->tableWidget->setItem(3,0,new QTableWidgetItem(metadata.getTelescope()));
//     ui->tableWidget->setItem(4,0,new QTableWidgetItem(metadata.getInstrument()));
//     ui->tableWidget->setItem(5,0,new QTableWidgetItem(QString::number(metadata.getExposureTime())));
//   }
//   else
//   {
//     ui->tableWidget->clearContents();
//   }
// }
