/********************************************************************************
 *                                                                              *
 * FitsIP - log book filter to select entries by image                          *
 *                                                                              *
 * modified: 2022-11-30                                                         *
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

#ifndef LOGBOOKFILTER_H
#define LOGBOOKFILTER_H

#include <QDateTime>
#include <QString>
#include <QStringList>

class LogbookEntry;

class LogbookFilter
{
public:
  LogbookFilter();

  bool check(const LogbookEntry& entry) const;

  void reset();

  void setProject(const QString& p);

  const QString& getProject() const;

  void setStep(const QString& s);

  const QString& getStep() const;

  void setFrom(QDateTime t);

  QDateTime getFrom() const;

  void setTo(QDateTime t);

  QDateTime getTo() const;

  void setImageList(const QStringList& list);

  const QStringList& getImageList() const;

private:
  QString project;
  QString step;
  QDateTime from;
  QDateTime to;
  QStringList images;
};

#endif // LOGBOOKFILTER_H
