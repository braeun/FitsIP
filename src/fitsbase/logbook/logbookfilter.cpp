/********************************************************************************
 *                                                                              *
 * FitsIP - log book filter to select entries by image                          *
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

#include "logbookfilter.h"
#include "logbookentry.h"

LogbookFilter::LogbookFilter()
{
}

bool LogbookFilter::check(const LogbookEntry &entry) const
{
  if (!project.isEmpty())
  {
    if (entry.getProject() != project) return false;
  }
  if (!step.isEmpty())
  {
    if (entry.getStep() != step) return false;
  }
  if (from.isValid() && to.isValid())
  {
    if (entry.getTimestamp() < from || entry.getTimestamp() > to) return false;
  }
  if (!images.empty())
  {
    if (!images.contains(entry.getImage())) return false;
  }
  return true;
}

void LogbookFilter::reset()
{
  project = "";
  step = "";
  from = QDateTime();
  to = QDateTime();
  images.clear();
}

void LogbookFilter::setProject(const QString& p)
{
  project = p;
}

const QString& LogbookFilter::getProject() const
{
  return project;
}

void LogbookFilter::setStep(const QString& s)
{
  step = s;
}

const QString& LogbookFilter::getStep() const
{
  return step;
}

void LogbookFilter::setFrom(QDateTime t)
{
  from = t;
}

QDateTime LogbookFilter::getFrom() const
{
  return from;
}

void LogbookFilter::setTo(QDateTime t)
{
  to = t;
}

QDateTime LogbookFilter::getTo() const
{
  return to;
}



void LogbookFilter::setImageList(const QStringList &list)
{
  images = list;
}

const QStringList& LogbookFilter::getImageList() const
{
  return images;
}
