/********************************************************************************
 *                                                                              *
 * FitsIP - metadata for the image                                              *
 *                                                                              *
 * modified: 2026-10-11                                                         *
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

#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <map>
#include <QString>
#include <QDateTime>
#include <QStringList>

class ImageMetadata
{
public:
  struct Entry
  {
    QString value;
    QString comment;
  };

  ImageMetadata();

  void setEntries(const std::map<QString,Entry>& map);

  const std::map<QString,Entry>& getEntries() const;

  void addEntry(const QString& key, const QString& value, const QString& comment="");

  bool hasEntry(const QString& key);

  QString getValue(const QString& key) const;

  QString getComment(const QString& key) const;

  void setHistory(const QStringList& h);

  const QStringList& getHistory() const;

  void addHistory(const QString& msg);

  // void setObject(const QString& s);

  // QString getObject() const;

  // void setObserver(const QString& s);

  // QString getObserver() const;

  // void setInstrument(const QString& s);

  // QString getInstrument() const;

  // void setTelescope(const QString& s);

  // QString getTelescope() const;

  // void setExposureTime(double t);

  // double getExposureTime() const;

  void setObsDateTime(QDateTime t);

  QDateTime getObsDateTime() const;

  static const char* keyObject;
  static const char* keyInstrument;
  static const char* keyTelescope;
  static const char* keyObserver;
  static const char* keyExpTime;
  static const char* keyDateObs;
  static const char* keyTimeObs;
private:
  std::map<QString,Entry> entries;
  QStringList history;
};

#endif // IMAGEMETADATA_H
