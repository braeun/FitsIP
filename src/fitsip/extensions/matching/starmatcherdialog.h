/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for plugin to match two images by stars                      *
 *                                                                              *
 * modified: 2025-10-26                                                         *
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

#ifndef STARMATCHERDIALOG_H
#define STARMATCHERDIALOG_H

#include <QDialog>

namespace Ui {
class StarMatcherDialog;
}

class ImageCollection;
class ImageSelectWidget;
class FitsObject;

class StarMatcherDialog : public QDialog
{
  Q_OBJECT

public:
  explicit StarMatcherDialog(QWidget *parent = nullptr);
  ~StarMatcherDialog();

  void setImageCollection(ImageCollection* collection);

  std::shared_ptr<FitsObject> getImage();

  bool isSubtractSky() const;

  bool isAllowRotation() const;

  int getSearchBoxSize() const;

  int getStarBoxSize() const;

  int getStarMaxMovement() const;

private:
  Ui::StarMatcherDialog *ui;
  ImageSelectWidget *imageSelectWidget;
};

#endif // STARMATCHERDIALOG_H
