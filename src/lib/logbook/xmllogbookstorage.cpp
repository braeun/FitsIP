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

#include "xmllogbookstorage.h"
#include "logbookentry.h"
#include "logbookfilter.h"
#include <QDomElement>
#include <QDomText>
#include <QFile>
#include <QFileInfo>
#include <stdexcept>

XMLLogbookStorage::XMLLogbookStorage(const QString& filename):
  file(new QFile(filename)),
  desc("")
{
  QFileInfo info(filename);
  title = info.baseName();
  read();
}

XMLLogbookStorage::~XMLLogbookStorage()
{
  if (file && file->isOpen())
  {
    if (file->isWritable()) file->write(doc.toString().toUtf8());
    file->close();
  }
}

bool XMLLogbookStorage::add(LogbookEntry entry)
{
  setId(entry,++idcounter);
  entries.push_back(entry);
  if (!entry.image.isEmpty()) images.insert(entry.image);
  projects.insert(entry.getProject());
  steps[entry.getProject()].insert(entry.getStep());
  QDomElement element = doc.createElement("entry");
  element.setAttribute("image",entry.image);
  element.setAttribute("timestamp",entry.timestamp.toString(Qt::ISODateWithMs));
  element.setAttribute("type",static_cast<int32_t>(entry.type));
  element.setAttribute("project",entry.getProject());
  element.setAttribute("step",entry.getStep());
  element.setAttribute("id",QString::number(entry.getId()));
  QDomText text = doc.createTextNode(entry.txt);
  element.appendChild(text);
  doc.documentElement().appendChild(element);
  write();
  return true;
}

void XMLLogbookStorage::setTitle(const QString &t)
{
  title = t;
  QDomNodeList list = doc.documentElement().elementsByTagName("title");
  if (list.isEmpty())
  {
    QDomElement element = doc.createElement("title");
    QDomText text = doc.createTextNode(title);
    element.appendChild(text);
    doc.documentElement().appendChild(element);
  }
  else
  {
    QDomElement element = list.at(0).toElement();
    element.clear();
    QDomText text = doc.createTextNode(title);
    element.appendChild(text);
  }
  write();
}

QString XMLLogbookStorage::getTitle() const
{
  return title;
}

void XMLLogbookStorage::setDescription(const QString &d)
{
  desc = d;
  QDomNodeList list = doc.documentElement().elementsByTagName("title");
  if (list.isEmpty())
  {
    QDomElement element = doc.createElement("description");
    QDomText text = doc.createTextNode(desc);
    element.appendChild(text);
    doc.documentElement().appendChild(element);
  }
  else
  {
    QDomElement element = list.at(0).toElement();
    element.clear();
    QDomText text = doc.createTextNode(desc);
    element.appendChild(text);
  }
  write();
}

QString XMLLogbookStorage::getDescription() const
{
  return desc;
}

std::vector<LogbookEntry> XMLLogbookStorage::getEntries() const
{
  return entries;
}

std::vector<LogbookEntry> XMLLogbookStorage::getEntries(const LogbookFilter& filter) const
{
  std::vector<LogbookEntry> list;
  for (const auto& e : entries)
  {
    if (filter.check(e)) list.push_back(e);
  }
  return list;
}

const LogbookEntry& XMLLogbookStorage::getLastEntry() const
{
  if (!entries.empty()) return entries.back();
  return LogbookEntry::invalid;
}

LogbookEntry XMLLogbookStorage::getEntry(int64_t id) const
{
  for (const auto& e : entries)
  {
    if (e.getId() == id) return e;
  }
  return LogbookEntry::invalid;
}

std::set<QString> XMLLogbookStorage::getImages() const
{
  return images;
}

std::set<QString> XMLLogbookStorage::getProjects() const
{
  return projects;
}

std::set<QString> XMLLogbookStorage::getSteps(const QString& project) const
{
  if (project.isEmpty())
  {
    std::set<QString> set;
    for (const auto& entry : steps)
    {
      set.insert(entry.second.begin(),entry.second.end());
      return set;
    }
  }
  if (steps.find(project) == steps.end()) return std::set<QString>{"generic"};
  return steps.at(project);
}

void XMLLogbookStorage::getTimeRange(QDateTime* begin, QDateTime* end) const
{
  if (entries.empty())
  {
    if (begin) *begin = QDateTime::currentDateTime();
    if (end) *end = QDateTime::currentDateTime();
  }
  if (begin)
  {
    QDateTime t = entries.front().timestamp;
    for (const auto& entry : entries)
    {
      if (entry.timestamp < t) t = entry.timestamp;
    }
    *begin = t;
  }
  if (end)
  {
    *end = getLastEntry().timestamp;
  }
}


void XMLLogbookStorage::assignProject(int64_t id, const QString &p)
{
  for (auto& e : entries)
  {
    if (e.getId() == id)
    {
      setProject(e,p);
      break;
    }
  }
  for (QDomNode n=doc.documentElement().firstChild();!n.isNull();n=n.nextSibling())
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
      if (e.tagName() == "entry")
      {
        if (e.attribute("id","-1").toLong() == id)
        {
          e.setAttribute("project",p);
          break;
        }
      }
    }
  }
  write();
}

void XMLLogbookStorage::assignStep(int64_t id, const QString &s)
{
  for (auto& e : entries)
  {
    if (e.getId() == id)
    {
      setStep(e,s);
      break;
    }
  }
  for (QDomNode n=doc.documentElement().firstChild();!n.isNull();n=n.nextSibling())
  {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
      if (e.tagName() == "entry")
      {
        if (e.attribute("id","-1").toLong() == id)
        {
          e.setAttribute("step",s);
          break;
        }
      }
    }
  }
  write();
}






void XMLLogbookStorage::read()
{
  idcounter = 0;
  if (file->exists())
  {
    if (!file->open(QIODevice::ReadOnly)) throw std::runtime_error("Failed to open logbook file");
    if (!doc.setContent(file)) throw std::runtime_error("Failed to read logbook file");
    entries.clear();
    for (QDomNode n=doc.documentElement().firstChild();!n.isNull();n=n.nextSibling())
    {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull())
      {
        if (e.tagName() == "entry")
        {
          QString project = e.attribute("project","generic");
          QString step = e.attribute("step","generic");
          QString image = e.attribute("image");
          QDateTime timestamp = QDateTime::fromString(e.attribute("timestamp"),Qt::ISODateWithMs);
          QString txt = e.text();
          LogbookEntry::Type type = static_cast<LogbookEntry::Type>(e.attribute("type","0").toInt());
          LogbookEntry entry(type,project,step,image,txt,timestamp);
          setId(entry,++idcounter);
          e.setAttribute("id",QString::number(entry.getId()));
          entries.push_back(entry);
          if (!image.isEmpty()) images.insert(image);
          projects.insert(project);
          steps[project].insert(step);
        }
        else if (e.tagName() == "title")
        {
          title = e.text();
        }
        else if (e.tagName() == "description")
        {
          desc = e.text();
        }
      }
    }
    file->close();
  }
  else
  {
    doc.clear();
    QDomElement root = doc.createElement("logbook");
    doc.appendChild(root);
  }
}

void XMLLogbookStorage::write()
{
  if (!file->open(QIODevice::WriteOnly)) throw std::runtime_error("Failed to open logbook file");
  file->write(doc.toString().toUtf8());
  file->close();
}

