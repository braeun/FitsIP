/********************************************************************************
 *                                                                              *
 * FitsIP - profile histogram                                                   *
 *                                                                              *
 * modified: 2025-01-10                                                         *
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

#ifndef PROFILE_H
#define PROFILE_H

#include <QPointF>
#include <QString>
#include <QVector>

class Profile: public QVector<QPointF>
{
public:
  Profile();

  void setCursorX(int v);

  int getCursorX() const;

  void setCursorY(int v);

  int getCursorY() const;

  bool save(const QString& fn);

private:
  int cursorx;
  int cursory;
};

#endif // PROFILE_H
