/********************************************************************************
 *                                                                              *
 * FitsIP - xml file based logbook data storage                                 *
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

#include "xmllogbookstorage.h"
#include "logbookentry.h"
#include <QDomElement>
#include <QDomText>
#include <QFile>
#include <QFileInfo>
#include <stdexcept>

XMLLogbookStorage::XMLLogbookStorage(const QString& filename):
  file(new QFile(filename))
{
  QFileInfo info(filename);
  AbstractLogbookStorage::setTitle(info.baseName());
  XMLLogbookStorage::read();
}

XMLLogbookStorage::~XMLLogbookStorage()
{
  if (file && file->isOpen())
  {
    if (file->isWritable()) file->write(doc.toString().toUtf8());
    file->close();
  }
}

bool XMLLogbookStorage::add(LogbookEntry& entry)
{
  AbstractLogbookStorage::add(entry);
  QDomElement element = doc.createElement("entry");
  element.setAttribute("image",entry.getImage());
  element.setAttribute("timestamp",entry.getTimestamp().toString(Qt::ISODateWithMs));
  element.setAttribute("type",static_cast<int32_t>(entry.getType()));
  element.setAttribute("project",entry.getProject());
  element.setAttribute("step",entry.getStep());
  element.setAttribute("id",QString::number(entry.getId()));
  QDomText text = doc.createTextNode(entry.getText());
  element.appendChild(text);
  doc.documentElement().appendChild(element);
  writeXML();
  return true;
}

bool XMLLogbookStorage::update(const LogbookEntry& entry)
{
  AbstractLogbookStorage::update(entry);
  for (QDomNode n=doc.documentElement().firstChild();!n.isNull();n=n.nextSibling())
  {
    QDomElement element = n.toElement(); // try to convert the node to an element.
    if(!element.isNull())
    {
      if (element.tagName() == "entry")
      {
        if (element.attribute("id","-1").toLong() == entry.getId())
        {
          element.setAttribute("image",entry.getImage());
          element.setAttribute("timestamp",entry.getTimestamp().toString(Qt::ISODateWithMs));
          element.setAttribute("type",static_cast<int32_t>(entry.getType()));
          element.setAttribute("project",entry.getProject());
          element.setAttribute("step",entry.getStep());
          break;
        }
      }
    }
  }
  writeXML();
  return true;
}

bool XMLLogbookStorage::remove(int64_t id)
{
  AbstractLogbookStorage::remove(id);
  for (QDomNode n=doc.documentElement().firstChild();!n.isNull();n=n.nextSibling())
  {
    QDomElement element = n.toElement(); // try to convert the node to an element.
    if(!element.isNull())
    {
      if (element.tagName() == "entry")
      {
        if (element.attribute("id","-1").toLong() == id)
        {
          doc.documentElement().removeChild(element);
          break;
        }
      }
    }
  }
  writeXML();
  return true;
}



void XMLLogbookStorage::setTitle(const QString &t)
{
  AbstractLogbookStorage::setTitle(t);
  QDomElement element = doc.documentElement().firstChildElement("title");
  if (element.isNull())
  {
    element = doc.createElement("title");
    doc.documentElement().appendChild(element);
  }
  else
  {
    element.removeChild(element.firstChild());
  }
  QDomText text = doc.createTextNode(t);
  element.appendChild(text);
  writeXML();
}

void XMLLogbookStorage::setDescription(const QString &d)
{
  AbstractLogbookStorage::setDescription(d);
  QDomElement element = doc.documentElement().firstChildElement("description");
  if (element.isNull())
  {
    element = doc.createElement("description");
    doc.documentElement().appendChild(element);
  }
  else
  {
    element.removeChild(element.firstChild());
  }
  QDomText text = doc.createTextNode(d);
  element.appendChild(text);
  writeXML();
}




void XMLLogbookStorage::read()
{
  if (file->exists())
  {
    if (!file->open(QIODevice::ReadOnly)) throw std::runtime_error("Failed to open logbook file");
    QDomDocument tmp;
    if (!tmp.setContent(file)) throw std::runtime_error("Failed to read logbook file");
    clear();
    doc = tmp;
    for (QDomNode n=doc.documentElement().firstChild();!n.isNull();n=n.nextSibling())
    {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if(!e.isNull())
      {
        if (e.tagName() == "entry")
        {
          QString project = e.attribute("project","generic");
          if (project.isEmpty()) project = "generic";
          QString step = e.attribute("step","generic");
          QString image = e.attribute("image");
          QDateTime timestamp = QDateTime::fromString(e.attribute("timestamp"),Qt::ISODateWithMs);
          QString txt = e.text();
          LogbookEntry::Type type = static_cast<LogbookEntry::Type>(e.attribute("type","0").toInt());
          LogbookEntry entry(type,project,step,image,txt,timestamp);
          AbstractLogbookStorage::add(entry);
          e.setAttribute("id",QString::number(entry.getId()));
        }
        else if (e.tagName() == "title")
        {
          AbstractLogbookStorage::setTitle(e.text());
        }
        else if (e.tagName() == "description")
        {
          AbstractLogbookStorage::setDescription(e.text());
        }
      }
    }
    file->close();
  }
  else
  {
    clear();
    doc.clear();
    QDomElement root = doc.createElement("logbook");
    doc.appendChild(root);
  }
}

void XMLLogbookStorage::writeXML()
{
  if (!file->open(QIODevice::WriteOnly)) throw std::runtime_error("Failed to open logbook file");
  file->write(doc.toString().toUtf8());
  file->close();
}

/*
 * This funtion is calles by the base class. We do not do anything here
 * as we use out own writeXML() function after changing the logbook
 */
void XMLLogbookStorage::write()
{
}
