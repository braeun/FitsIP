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

#ifndef LOGBOOKENTRY_H
#define LOGBOOKENTRY_H

#include <QDateTime>

class LogbookStorage;

class LogbookEntry
{
public:

  enum Type { Op, Note };

  LogbookEntry();
  LogbookEntry(Type type, const QString& project, const QString& step, const QString& image, const QString& txt, QDateTime timestamp=QDateTime::currentDateTime());

  LogbookEntry(const LogbookEntry&) = default;
  LogbookEntry(LogbookEntry&&) = default;
  LogbookEntry& operator=(const LogbookEntry&) = default;

  bool isValid() const;

  int64_t getId() const;

  const QString& getImage() const;

  void setImage(const QString& newImage);

  const QString& getText() const;

  void setText(const QString& newTxt);

  const QDateTime& getTimestamp() const;

  void setTimestamp(const QDateTime& newTimestamp);

  Type getType() const;

  void setType(Type newType);

  const QString& getProject() const;

  void setProject(const QString& p);

  const QString& getStep() const;

  void setStep(const QString& s);

  QString getTypeString() const;

  static const LogbookEntry invalid;

private:
  friend class LogbookStorage;

  void setId(int64_t v);

  int64_t id; //!< unique id
  QString image;
  QString txt;
  QDateTime timestamp;
  Type type;
  QString project;
  QString step;


};

#endif // LOGBOOKENTRY_H
