/********************************************************************************
 *                                                                              *
 * FitsIP - single entry in the log book                                        *
 *                                                                              *
 * modified: 2022-11-27                                                         *
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

#include "logbookentry.h"

const LogbookEntry LogbookEntry::invalid;

LogbookEntry::LogbookEntry():
  image(""),
  txt(""),
  timestamp(QDateTime::fromMSecsSinceEpoch(0)),
  type(Op),
  id(-1),
  project("generic"),
  step("generic")
{
}

LogbookEntry::LogbookEntry(Type type, const QString& project, const QString& step, const QString& image, const QString& txt, QDateTime timestamp):
  image(image),
  txt(txt),
  timestamp(timestamp),
  type(type),
  id(-1),
  project(project),
  step(step)
{
}

bool LogbookEntry::isValid() const
{
  return timestamp.toSecsSinceEpoch() > 0;
}

int64_t LogbookEntry::getId() const
{
  return id;
}

void LogbookEntry::setId(int64_t v)
{
  id = v;
}

void LogbookEntry::setProject(const QString& p)
{
  project = p;
}

const QString& LogbookEntry::getProject() const
{
  return project;
}

void LogbookEntry::setStep(const QString& s)
{
  step = s;
}

const QString& LogbookEntry::getStep() const
{
  return step;
}

QString LogbookEntry::getTag() const
{
  if (image.isEmpty())
  {
    switch (type)
    {
      case LogbookEntry::Note:
      default:
        return "Note";
      case LogbookEntry::Op:
        return "Op";
    }
  }
  return image;
}



