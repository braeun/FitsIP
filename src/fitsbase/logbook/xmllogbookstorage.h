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

#ifndef XMLLOGBOOKSTORAGE_H
#define XMLLOGBOOKSTORAGE_H

#include "abstractlogbookstorage.h"
#include <QDomDocument>
#include <set>
#include <map>

class QFile;

class XMLLogbookStorage: public AbstractLogbookStorage
{
public:
  XMLLogbookStorage(const QString& filename);
  ~XMLLogbookStorage();

  virtual bool add(LogbookEntry& entry) override;

  virtual bool update(const LogbookEntry& entry) override;

  virtual bool remove(int64_t id) override;

  virtual void setTitle(const QString& title) override;

  virtual void setDescription(const QString& desc) override;

private:
  virtual void read() override;
  virtual void write() override;
  void writeXML();

  QDomDocument doc;
  QFile* file;
};

#endif // XMLLOGBOOKSTORAGE_H
