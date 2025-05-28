/********************************************************************************
 *                                                                              *
 * FitsIP - widget to display the profiles and associated controls              *
 *                                                                              *
 * modified: 2025-01-04                                                         *
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

#ifndef PROFILEVIEW_H
#define PROFILEVIEW_H

#include <fitsip/core/profile.h>
#include <QMenu>
#include <QWidget>
#include <memory>

class FitsObject;
class ProfileChart;

namespace Ui {
class ProfileView;
}

class ProfileView : public QWidget
{
  Q_OBJECT

public:
  explicit ProfileView(QWidget *parent = nullptr);
  ~ProfileView();

  bool getClickEndsTracking() const;

  void setClickEndsTracking(bool newClickEndsTracking);

  void setImage(std::shared_ptr<FitsObject> obj);

public slots:
  void updateCursor(QPoint p);

  void setCursor(QPoint p);

protected:
  void changeEvent(QEvent* event);

private:
  void redraw();
  void logYToggled(bool checked);
  void settingsChanged();
  void save();

  Ui::ProfileView *ui;
  ProfileChart *horizontalProfileWidget;
  ProfileChart *verticalProfileWidget;
  std::shared_ptr<FitsObject> image;
  QPoint cursor;
  Profile horizontal;
  Profile vertical;
  bool clickEndsTracking;
  QMenu* popupMenu;
};

#endif // PROFILEVIEW_H
