/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for logbook data storage                         *
 *                                                                              *
 * modified: 2025-04-12                                                         *
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

#ifndef LOGBOOKSTORAGE_H
#define LOGBOOKSTORAGE_H

#include <QIODevice>
#include <QString>
#include <set>
#include <vector>

class LogbookEntry;
class LogbookFilter;

class LogbookStorage
{
public:

  virtual bool add(LogbookEntry& entry) = 0;

  virtual bool update(const LogbookEntry& entry) = 0;

  virtual bool remove(int64_t id) = 0;

  virtual void setTitle(const QString& title) = 0;

  virtual QString getTitle() const = 0;

  virtual void setDescription(const QString& desc) = 0;

  virtual QString getDescription() const = 0;

  virtual std::vector<LogbookEntry> getEntries() const = 0;

  virtual std::vector<LogbookEntry> getEntries(const LogbookFilter& filter) const = 0;

  virtual const LogbookEntry& getLastEntry() const = 0;

  virtual LogbookEntry getEntry(int64_t id) const = 0;

  virtual std::set<QString> getImages() const = 0;

  virtual std::set<QString> getProjects() const = 0;

  virtual std::set<QString> getSteps(const QString& project) const = 0;

  virtual void getTimeRange(QDateTime* begin, QDateTime* end) const = 0;

//  virtual void assignProject(int64_t id, const QString& p) = 0;

//  virtual void assignStep(int64_t id, const QString& s) = 0;


protected:
  LogbookStorage();

  void setId(LogbookEntry& entry, int64_t v);

  void setProject(LogbookEntry& entry, const QString& p);

  void setStep(LogbookEntry& entry, const QString& s);

};

#endif // LOGBOOKSTORAGE_H
