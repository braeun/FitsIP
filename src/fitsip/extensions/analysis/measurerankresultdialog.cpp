/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for the result of the ranking by sharpness                   *
 *                                                                              *
 * modified: 2022-12-02                                                         *
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

#include "measurerankresultdialog.h"
#include "ui_measurerankresultdialog.h"
#include "measurerank.h"
#include <fitsip/core/settings.h>
#include <fitsip/core/io/iofactory.h>
#include <QTextStream>

MeasureRankResultDialog::MeasureRankResultDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::MeasureRankResultDialog)
{
  ui->setupUi(this);
  ui->resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
}

MeasureRankResultDialog::~MeasureRankResultDialog()
{
  delete ui;
}

void MeasureRankResultDialog::setResult(const std::vector<RankEntry> &list)
{
  entries = list;
  filelist.clear();
  ui->resultTable->clearContents();
  ui->resultTable->setRowCount(entries.size());
  int32_t row = 0;
  for (const RankEntry& entry : entries)
  {
    ui->resultTable->setItem(row,0,new QTableWidgetItem(entry.info.fileName()));
    ui->resultTable->setItem(row,1,new QTableWidgetItem(QString::number(entry.result)));
    row++;
  }
}

const std::vector<QFileInfo>& MeasureRankResultDialog::getFileList() const
{
  return filelist;
}




void MeasureRankResultDialog::on_copyButton_clicked()
{
  filelist.clear();
  for (const RankEntry& e : entries)
  {
    filelist.push_back(e.info);
  }
}

void MeasureRankResultDialog::on_removeRowsButton_clicked()
{
  std::vector<RankEntry> list;
  for (int32_t row=0;row<ui->resultTable->rowCount();row++)
  {
    if (!ui->resultTable->selectionModel()->isRowSelected(row,QModelIndex()))
    {
      list.push_back(entries[row]);
    }
  }
  setResult(list);
}

void MeasureRankResultDialog::on_saveButton_clicked()
{
  QString filter;
  QString fn = Settings().getSaveFilename(this,Settings::PATH_IMAGE,"Text files (*.txt)",&filter);
  if (!fn.isEmpty())
  {
    fn = IOFactory::assertSuffix(fn,filter);
    QFile file(fn);
    if (file.open(QIODevice::WriteOnly))
    {
      QTextStream s(&file);
      for (const RankEntry& entry : entries)
      {
        s << entry.info.absoluteFilePath() << "," << entry.result << Qt::endl;
      }
      s.flush();
      file.close();
    }
  }
}

void MeasureRankResultDialog::on_logButton_clicked()
{
  emit writeToLogbook();
}
