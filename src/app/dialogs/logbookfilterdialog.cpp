/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to set the log book filter                                   *
 *                                                                              *
 * modified: 2022-11-30                                                         *
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

#include "logbookfilterdialog.h"
#include "ui_logbookfilterdialog.h"
#include <fitsbase/logbook/logbook.h>
#include <fitsbase/logbook/logbookfilter.h>
#include <QDebug>

LogbookFilterDialog::LogbookFilterDialog(const Logbook* logbook, const LogbookFilter& filter, QWidget *parent): QDialog(parent),
  ui(new Ui::LogbookFilterDialog)
{
  ui->setupUi(this);
  std::set<QString> images = logbook->getImages();
  for (const QString& image : filter.getImageList())
  {
    images.erase(image);
    ui->selectedImagesList->addItem(image);
  }
  for (const QString& image : logbook->getImages())
  {
    ui->imagesList->addItem(image);
  }
}

LogbookFilterDialog::~LogbookFilterDialog()
{
  delete ui;
}

LogbookFilter LogbookFilterDialog::getFilter() const
{
  LogbookFilter filter;
  QStringList images;
  for (int row=0;row<ui->selectedImagesList->count();++row)
  {
    images.push_back(ui->selectedImagesList->item(row)->text());
  }
  filter.setImageList(images);
  return filter;
}

void LogbookFilterDialog::on_addImageButton_clicked()
{
  QList<QListWidgetItem*> items = ui->imagesList->selectedItems();
  for (QListWidgetItem* item : items)
  {
    qDebug() << item->text();
    ui->imagesList->takeItem(ui->imagesList->row(item));
    ui->selectedImagesList->addItem(item);
  }
  qDebug() << "Add image(s)";
}

void LogbookFilterDialog::on_removeImageButton_clicked()
{
  QList<QListWidgetItem*> items = ui->selectedImagesList->selectedItems();
  for (QListWidgetItem* item : items)
  {
    qDebug() << item->text();
    ui->selectedImagesList->takeItem(ui->imagesList->row(item));
    ui->imagesList->addItem(item);
  }
}
