/********************************************************************************
 *                                                                              *
 * FitsIP - metadata for the image                                              *
 *                                                                              *
 * modified: 2025-03-14                                                         *
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

  QString getValue(const QString& key) const;

  QString getComment(const QString& key) const;

  void setHistory(const QStringList& h);

  const QStringList& getHistory() const;

  void addHistory(const QString& msg);

  void setObject(const QString& s);

  QString getObject() const;

  void setObserver(const QString& s);

  QString getObserver() const;

  void setInstrument(const QString& s);

  QString getInstrument() const;

  void setTelescope(const QString& s);

  QString getTelescope() const;

  void setExposureTime(double t);

  double getExposureTime() const;

  void setObsDateTime(QDateTime t);

  QDateTime getObsDateTime() const;

private:
  std::map<QString,Entry> entries;
  QStringList history;
};

#endif // IMAGEMETADATA_H
