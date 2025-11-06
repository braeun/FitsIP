/********************************************************************************
 *                                                                              *
 * FitsIP - combine RGB channels - interactive dialog                           *
 *                                                                              *
 * modified: 2024-12-12                                                         *
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

#include "opcombinechannelsdialog.h"
#include "ui_opcombinechannelsdialog.h"

OpCombineChannelsDialog::OpCombineChannelsDialog(QWidget *parent):
  QDialog(parent),
  ui(new Ui::OpCombineChannelsDialog)
{
  ui->setupUi(this);
}

OpCombineChannelsDialog::~OpCombineChannelsDialog()
{
  delete ui;
}

void OpCombineChannelsDialog::setCollection(const ImageCollection& collection)
{
  ui->redChannelBox->clear();
  ui->greenChannelBox->clear();
  ui->blueChannelBox->clear();
  for (const auto& img : collection.getFiles())
  {
    ui->redChannelBox->addItem(img->getImage().getName(),img->getId());
    ui->greenChannelBox->addItem(img->getImage().getName(),img->getId());
    ui->blueChannelBox->addItem(img->getImage().getName(),img->getId());
  }
}

int OpCombineChannelsDialog::getRedImage() const
{
  return ui->redChannelBox->currentData().toInt();
}

int OpCombineChannelsDialog::getGreenImage() const
{
  return ui->greenChannelBox->currentData().toInt();
}

int OpCombineChannelsDialog::getBlueImage() const
{
  return ui->blueChannelBox->currentData().toInt();
}

