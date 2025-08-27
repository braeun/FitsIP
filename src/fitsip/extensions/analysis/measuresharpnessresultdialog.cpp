/********************************************************************************
 *                                                                              *
 * FitsIP - measure the sharpness of images - result dialog                     *
 *                                                                              *
 * modified: 2025-02-12                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of FitsIP.                                                 *
 * FitsIP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * FitsIP is distributed in the hope that it will be useful, but                *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * FitsIP. If not, see <https://www.gnu.org/licenses/>.                         *
 ********************************************************************************/

#include "measuresharpnessresultdialog.h"
#include "ui_measuresharpnessresultdialog.h"
#include "measuresharpness.h"
#include <fitsip/core/settings.h>
#include <fitsip/core/io/iofactory.h>
#include <fitsip/core/math/average.h>
#include <QTextStream>

MeasureSharpnessResultDialog::MeasureSharpnessResultDialog(QWidget *parent):QDialog(parent),
    ui(new Ui::MeasureSharpnessResultDialog)
{
  ui->setupUi(this);
  ui->resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
}

MeasureSharpnessResultDialog::~MeasureSharpnessResultDialog()
{
  delete ui;
}

void MeasureSharpnessResultDialog::setResult(const std::vector<SharpnessData> &list)
{
  entries = list;
  ui->resultTable->clearContents();
  ui->resultTable->setRowCount(entries.size());
  Average normvaravg;
  int32_t row = 0;
  for (const SharpnessData& entry : entries)
  {
    ui->resultTable->setItem(row,0,new QTableWidgetItem(entry.info.fileName()));
    ui->resultTable->setItem(row,1,new QTableWidgetItem(QString::number(entry.min)));
    ui->resultTable->setItem(row,2,new QTableWidgetItem(QString::number(entry.max)));
    ui->resultTable->setItem(row,3,new QTableWidgetItem(QString::number(entry.mean)));
    ui->resultTable->setItem(row,4,new QTableWidgetItem(QString::number(entry.variance)));
    ui->resultTable->setItem(row,5,new QTableWidgetItem(QString::number(entry.minPixel)));
    ui->resultTable->setItem(row,6,new QTableWidgetItem(QString::number(entry.maxPixel)));
    ui->resultTable->setItem(row,7,new QTableWidgetItem(QString::number(entry.normalizedVariance)));
    row++;
    normvaravg.add(entry.normalizedVariance);
  }
  ui->normVarianceMeanLabel->setText(QString::number(normvaravg.getMean()));
  ui->normVarianceStddevLabel->setText(QString::number(sqrt(normvaravg.getVariance())));
}

std::vector<QFileInfo> MeasureSharpnessResultDialog::getFileList() const
{
  std::vector<QFileInfo> filelist;
  filelist.clear();
  for (const SharpnessData& e : entries)
  {
    filelist.push_back(e.info);
  }
  return filelist;
}




void MeasureSharpnessResultDialog::on_removeRowsButton_clicked()
{
  std::vector<SharpnessData> list;
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

void MeasureSharpnessResultDialog::on_saveButton_clicked()
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
        for (const SharpnessData& entry : entries)
        {
          s << entry.info.absoluteFilePath() << Qt::endl;
        }
      }
      else
      {
        for (const SharpnessData& entry : entries)
        {
          s << entry.info.absoluteFilePath() << "," << entry.min << "," << entry.max << "," << entry.mean << "," << entry.variance << "," << entry.minPixel << "," << entry.maxPixel << "," << entry.normalizedVariance << Qt::endl;
        }
      }
      s.flush();
      file.close();
    }
  }
}

void MeasureSharpnessResultDialog::on_logButton_clicked()
{
  emit writeToLogbook();
}

