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

#include "logbook.h"
#include "logbookfilter.h"
#include "logbookstorage.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

Logbook::Logbook():QObject(),
  active(true),
  project("generic"),
  step("generic")
{
}

Logbook::~Logbook()
{
}

bool Logbook::isActive() const
{
  return active && isOpen();
}

bool Logbook::isOpen() const
{
  return store.get() != nullptr;
}

void Logbook::open(std::unique_ptr<LogbookStorage>& s)
{
  store = std::move(s);
}

void Logbook::close()
{
  store.reset();
}

void Logbook::setTitle(const QString& title)
{
  if (store) store->setTitle(title);
}

QString Logbook::getTitle() const
{
  if (store) return store->getTitle();
  return "not opened";
}

void Logbook::setDescription(const QString& desc)
{
  if (store) store->setDescription(desc);
}

QString Logbook::getDescription() const
{
  if (store) return store->getDescription();
  return "";
}

void Logbook::add(LogbookEntry::Type type, QString image, QString txt)
{
  if (store && active)
  {
    LogbookEntry e(type,project,step,image,txt);
    store->add(e);
    emit dataAdded();
  }
}

void Logbook::add(QString project, LogbookEntry::Type type, QString image, QString txt)
{
  if (store && active)
  {
    LogbookEntry e(type,project,step,image,txt);
    store->add(e);
    emit dataAdded();
  }
}

void Logbook::addOp(QString image, QString op)
{
  if (store && active)
  {
    add(LogbookEntry::Op,image,op);
  }
}

void Logbook::update(const LogbookEntry& entry)
{
  if (store)
  {
    store->update(entry);
    emit dataChanged();
  }
}

void Logbook::remove(int64_t id)
{
  if (store)
  {
    store->remove(id);
    emit dataChanged();
  }
}

std::vector<LogbookEntry> Logbook::getEntries() const
{
  if (store) return store->getEntries();
  return std::vector<LogbookEntry>();
}

std::vector<LogbookEntry> Logbook::getEntries(const LogbookFilter& filter) const
{
  if (store) return store->getEntries(filter);
  return std::vector<LogbookEntry>();
}

const LogbookEntry& Logbook::getLastEntry() const
{
  if (store) return store->getLastEntry();
  return LogbookEntry::invalid;
}

LogbookEntry Logbook::getEntry(int64_t id) const
{
  if (store) return store->getEntry(id);
  return LogbookEntry::invalid;
}

std::set<QString> Logbook::getImages() const
{
  if (store) return store->getImages();
  return std::set<QString>();
}

std::set<QString> Logbook::getProjects() const
{
  if (store) return store->getProjects();
  return std::set<QString>{"generic"};
}

std::set<QString> Logbook::getSteps(const QString& project) const
{
  if (store) return store->getSteps(project);
  return std::set<QString>{"generic"};
}

void Logbook::getTimeRange(QDateTime *begin, QDateTime *end) const
{
  if (store) return store->getTimeRange(begin,end);
  if (begin) *begin = QDateTime::currentDateTime();
  if (end) *end = QDateTime::currentDateTime();
}

void Logbook::setProject(const QString& p)
{
  project = p;
}

const QString& Logbook::getProject() const
{
  return project;
}

void Logbook::setStep(const QString& s)
{
  step = s;
}

const QString& Logbook::getStep() const
{
  return step;
}

//void Logbook::assignProject(int64_t id, const QString& p)
//{
//  if (store) store->assignProject(id,p);
//}

//void Logbook::assignStep(int64_t id, const QString& s)
//{
//  if (store) store->assignStep(id,s);
//}

bool Logbook::exportToFile(const QString &file)
{
  if (!store) return false;
  return exportPlainText(file);
}




void Logbook::activate(bool flag)
{
  active = flag;
  emit activated(flag);
}





bool Logbook::exportPlainText(const QString &file)
{
  QFile f(file);
  if (!f.open(QIODevice::WriteOnly)) return false;
  QTextStream os(&f);
  QString t = getTitle();
  int n = (80 - t.size()) / 2;
  if (n > 0) os << QString(n,' ');
  os << t << Qt::endl << Qt::endl;
  os << "Description:" << Qt::endl;
  os << getDescription() << Qt::endl;
  os << Qt::endl;
  QDate lastDate;
  for (const LogbookEntry& e : getEntries())
  {
    if (e.getTimestamp().date() != lastDate)
    {
      lastDate = e.getTimestamp().date();
      os << Qt::endl;
      os << lastDate.toString(Qt::ISODate) << Qt::endl;
    }
    os << "  ";
    os << e.getTimestamp().time().toString(Qt::ISODateWithMs) << ": ";
    os << "Project " << e.getProject() << "; ";
    os << "Step " << e.getStep() << "; ";
    os << e.getTypeString() << Qt::endl;
    os << "    " << e.getText() << Qt::endl;
  }
  os.flush();
  f.close();
  return true;
}



