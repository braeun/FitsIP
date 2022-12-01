/********************************************************************************
 *                                                                              *
 * FitsIP - xml file based logbook data storage                                 *
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

#ifndef XMLLOGBOOKSTORAGE_H
#define XMLLOGBOOKSTORAGE_H

#include "logbookstorage.h"
#include <QDomDocument>
#include <set>
#include <map>

class QFile;

class XMLLogbookStorage: public LogbookStorage
{
public:
  XMLLogbookStorage(const QString& filename);
  ~XMLLogbookStorage();

  virtual bool add(LogbookEntry entry) override;

  virtual void setTitle(const QString& title) override;

  virtual QString getTitle() const override;

  virtual void setDescription(const QString& desc) override;

  virtual QString getDescription() const override;

  virtual std::vector<LogbookEntry> getEntries() const override;

  virtual std::vector<LogbookEntry> getEntries(const LogbookFilter& filter) const override;

  virtual const LogbookEntry& getLastEntry() const override;

  virtual LogbookEntry getEntry(int64_t id) const override;

  virtual std::set<QString> getImages() const override;

  virtual std::set<QString> getProjects() const override;

  virtual std::set<QString> getSteps(const QString& project) const override;

  virtual void getTimeRange(QDateTime* begin, QDateTime* end) const override;

  virtual void assignProject(int64_t id, const QString& p) override;

  virtual void assignStep(int64_t id, const QString& s) override;

private:
  void read();
  void write();

  QDomDocument doc;
  QFile* file;
  int64_t idcounter;
  std::vector<LogbookEntry> entries;
  std::set<QString> images;
  std::set<QString> projects;
  std::map<QString,std::set<QString>> steps;
  QString title;
  QString desc;
};

#endif // XMLLOGBOOKSTORAGE_H
