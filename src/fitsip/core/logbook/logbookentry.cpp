/********************************************************************************
 *                                                                              *
 * FitsIP - single entry in the log book                                        *
 *                                                                              *
 * modified: 2024-12-14                                                         *
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

LogbookEntry::LogbookEntry():
  id(-1),
  image(""),
  txt(""),
  timestamp(QDateTime::fromMSecsSinceEpoch(0)),
  type(Op),
  project("generic"),
  step("generic")
{
}

LogbookEntry::LogbookEntry(Type type, const QString& project, const QString& step, const QString& image, const QString& txt, QDateTime timestamp):
  id(-1),
  image(image),
  txt(txt),
  timestamp(timestamp),
  type(type),
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

const QString& LogbookEntry::getImage() const
{
  return image;
}

void LogbookEntry::setImage(const QString& newImage)
{
  image = newImage;
}

const QString& LogbookEntry::getText() const
{
  return txt;
}

void LogbookEntry::setText(const QString& newTxt)
{
  txt = newTxt;
}

const QDateTime& LogbookEntry::getTimestamp() const
{
  return timestamp;
}

void LogbookEntry::setTimestamp(const QDateTime& newTimestamp)
{
  timestamp = newTimestamp;
}

LogbookEntry::Type LogbookEntry::getType() const
{
  return type;
}

void LogbookEntry::setType(Type newType)
{
  type = newType;
}

const QString& LogbookEntry::getStep() const
{
  return step;
}

QString LogbookEntry::getTypeString() const
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



