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

#ifndef OPCOMBINECHANNELSDIALOG_H
#define OPCOMBINECHANNELSDIALOG_H

#include <QDialog>
#include <fitsbase/imagecollection.h>

namespace Ui {
class OpCombineChannelsDialog;
}

class OpCombineChannelsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpCombineChannelsDialog(QWidget *parent = nullptr);
  ~OpCombineChannelsDialog();

  void setCollection(const ImageCollection& collection);

  int getRedImage() const;

  int getGreenImage() const;

  int getBlueImage() const;

private:
  Ui::OpCombineChannelsDialog *ui;
};

#endif // OPCOMBINECHANNELSDIALOG_H
