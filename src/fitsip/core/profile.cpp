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

#include "profile.h"
#include <QFile>
#include <QTextStream>

Profile::Profile()
{
}

void Profile::setCursorX(int v)
{
  cursorx = v;
}

int Profile::getCursorX() const
{
  return cursorx;
}

void Profile::setCursorY(int v)
{
  cursory = v;
}

int Profile::getCursorY() const
{
  return cursory;
}

bool Profile::save(const QString& fn)
{
  QFile data(fn);
  if (data.open(QFile::WriteOnly | QFile::Truncate))
  {
      QTextStream out(&data);
      for (const QPointF& pt : *this)
      {
        out << pt.x() << ", " << pt.y() << Qt::endl;
      }
      data.close();
      return true;
  }
  return false;
}
