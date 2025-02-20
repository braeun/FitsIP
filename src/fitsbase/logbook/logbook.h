/********************************************************************************
 *                                                                              *
 * FitsIP - log book for logging image processing steps                         *
 *                                                                              *
 * modified: 2025-02-15                                                         *
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

#ifndef LOGBOOK_H
#define LOGBOOK_H

#include "logbookentry.h"
#include <QObject>
#include <functional>
#include <memory>
#include <vector>
#include <set>

class LogbookFilter;
class LogbookStorage;

class Logbook: public QObject
{
  Q_OBJECT
public:
  Logbook();
  ~Logbook();

  /**
   * @brief Returns if the logbook is active.
   *
   * A logbook is considered active if it is open and the active flag is set
   * to true.
   * @return true if the logbook is active
   */
  bool isActive() const;

  /**
   * @brief Returns if the logbook is open.
   * @return true if the logbook is open
   */
  bool isOpen() const;

  void open(std::unique_ptr<LogbookStorage>& s);

  void close();

  void setTitle(const QString& title);

  QString getTitle() const;

  void setDescription(const QString& desc);

  QString getDescription() const;

  void add(LogbookEntry::Type type, QString image, QString txt);

  void add(QString project, LogbookEntry::Type type, QString image, QString txt);

  void addOp(QString image, QString op);

  void update(const LogbookEntry& entry);

  void remove(int64_t id);

  std::vector<LogbookEntry> getEntries() const;

  std::vector<LogbookEntry> getEntries(const LogbookFilter& filter) const;

  const LogbookEntry& getLastEntry() const;

  LogbookEntry getEntry(int64_t id) const;

  std::set<QString> getImages() const;

  std::set<QString> getProjects() const;

  std::set<QString> getSteps(const QString& project) const;

  void getTimeRange(QDateTime* begin, QDateTime* end) const;

  void setProject(const QString& p);

  const QString& getProject() const;

  void setStep(const QString& s);

  const QString& getStep() const;

//  void assignProject(int64_t id, const QString& p);

//  void assignStep(int64_t id, const QString& s);

  bool exportToFile(const QString& file);

signals:
  void dataAdded();

  void dataChanged();

  void activated(bool flag);

public slots:

  void activate(bool flag);

private:
  bool exportPlainText(const QString& file);

  bool active;
  std::unique_ptr<LogbookStorage> store;
  QString project;
  QString step;

};

#endif // LOGBOOK_H
