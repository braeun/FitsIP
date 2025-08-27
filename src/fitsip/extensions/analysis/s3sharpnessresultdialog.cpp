#include "s3sharpnessresultdialog.h"
#include "ui_s3sharpnessresultdialog.h"
#include "s3sharpness.h"
#include <fitsip/core/settings.h>
#include <fitsip/core/io/iofactory.h>
#include <fitsip/core/math/average.h>
#include <QTextStream>


S3SharpnessResultDialog::S3SharpnessResultDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::S3SharpnessResultDialog)
{
  ui->setupUi(this);
  ui->resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
  connect(ui->removeRowsButton,&QAbstractButton::clicked,this,&S3SharpnessResultDialog::removeRows);
  connect(ui->saveButton,&QAbstractButton::clicked,this,&S3SharpnessResultDialog::save);
  connect(ui->logButton,&QAbstractButton::clicked,this,&S3SharpnessResultDialog::toLogbook);
}

S3SharpnessResultDialog::~S3SharpnessResultDialog()
{
  delete ui;
}

void S3SharpnessResultDialog::setResult(const std::vector<S3SharpnessData> &list)
{
  entries = list;
  ui->resultTable->clearContents();
  ui->resultTable->setRowCount(entries.size());
  Average normvaravg;
  int32_t row = 0;
  for (const S3SharpnessData& entry : entries)
  {
    ui->resultTable->setItem(row,0,new QTableWidgetItem(entry.info.fileName()));
    ui->resultTable->setItem(row,1,new QTableWidgetItem(QString::number(entry.s3)));
    row++;
    normvaravg.add(entry.s3);
  }
  ui->normVarianceMeanLabel->setText(QString::number(normvaravg.getMean()));
  ui->normVarianceStddevLabel->setText(QString::number(sqrt(normvaravg.getVariance())));
}

std::vector<QFileInfo> S3SharpnessResultDialog::getFileList() const
{
  std::vector<QFileInfo> filelist;
  filelist.clear();
  for (const S3SharpnessData& e : entries)
  {
    filelist.push_back(e.info);
  }
  return filelist;
}




void S3SharpnessResultDialog::removeRows()
{
  std::vector<S3SharpnessData> list;
  for (int32_t row=0;row<ui->resultTable->rowCount();row++)
  {
    if (!ui->resultTable->selectionModel()->isRowSelected(row,QModelIndex()))
    {
      list.push_back(entries[row]);
    }
  }
  setResult(list);
  emit fileListChanged();
}

void S3SharpnessResultDialog::save()
{
  QString filter;
  QString fn = Settings().getSaveFilename(this,Settings::PATH_IMAGE,IOFactory::filelist_filter+QString(";;")+IOFactory::csv_filter+QString(";;")+IOFactory::all_files_filter,&filter);
  if (!fn.isEmpty())
  {
    fn = IOFactory::assertSuffix(fn,filter);
    QFile file(fn);
    if (file.open(QIODevice::WriteOnly))
    {
      QTextStream s(&file);
      if (filter == IOFactory::filelist_filter)
      {
        for (const S3SharpnessData& entry : entries)
        {
          s << entry.info.absoluteFilePath() << Qt::endl;
        }
      }
      else
      {
        for (const S3SharpnessData& entry : entries)
        {
          s << entry.info.absoluteFilePath() << "," << entry.s3 << Qt::endl;
        }
      }
      s.flush();
      file.close();
    }
  }
}

void S3SharpnessResultDialog::toLogbook()
{
  emit writeToLogbook();
}

