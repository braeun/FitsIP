/********************************************************************************
 *                                                                              *
 * FitsIP - widget to select an image from memory or filesystem                 *
 *                                                                              *
 * modified: 2024-12-13                                                         *
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

#include "imageselectwidget.h"
#include "ui_imageselectwidget.h"
#include "../imagecollection.h"
#include "../fitsobject.h"
#include "../settings.h"
#include "../io/iofactory.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

ImageSelectWidget::ImageSelectWidget(QWidget *parent):QWidget(parent),
  ui(new Ui::ImageSelectWidget),
  collection(nullptr)
{
  ui->setupUi(this);
  ui->noneButton->setVisible(false);
}

ImageSelectWidget::~ImageSelectWidget()
{
  delete ui;
}

void ImageSelectWidget::setNoneAllowed(bool flag)
{
  ui->noneButton->setVisible(flag);
}

void ImageSelectWidget::setImageCollection(ImageCollection* c)
{
  collection = c;
  ui->fileList->setModel(collection);
}

std::shared_ptr<FitsObject> ImageSelectWidget::getImage()
{
  if (ui->fromMemoryButton->isChecked())
  {
    QModelIndex index = ui->fileList->currentIndex();
    if (index.isValid()) return collection->getFiles()[index.row()];
  }
  else if (ui->fromFileButton->isChecked())
  {
    QString fn = ui->filenameField->text();
    if (!fn.isEmpty())
    {
      IOHandler* handler = IOFactory::getInstance()->getHandler(fn);
      if (handler)
      {
        std::shared_ptr<FitsImage> image = handler->read(fn);
        std::shared_ptr<FitsObject> file = std::make_shared<FitsObject>(image,fn);
        return file;
      }
      else
      {
        qWarning() << "No IOHandler for " << fn;
      }
    }
  }
  return std::shared_ptr<FitsObject>();
}




void ImageSelectWidget::on_browseButton_clicked()
{
  QSettings settings;
  QString path = settings.value(Settings::PATH_IMAGE,".").toString();
  QString fn = QFileDialog::getOpenFileName(this,QApplication::applicationDisplayName(),path,IOFactory::getInstance()->getReadFilters());
  if (!fn.isNull())
  {
    ui->filenameField->setText(fn);
  }
}
