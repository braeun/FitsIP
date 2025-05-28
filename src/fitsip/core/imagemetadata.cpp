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

#include "imagemetadata.h"

static const char* keyObject = "OBJECT";
static const char* keyInstrument = "INSTRUME";
static const char* keyTelescope = "TELESCOP";
static const char* keyObserver = "OBSERVER";
static const char* keyExpTime = "EXPTIME";
static const char* keyDateObs = "DATE-OBS";
static const char* keyTimeObs = "TIME-OBS";
/* non standard keywords */
static const char* keyPreFFTWidth = "PREFFTW";
static const char* keyPreFFTHeight = "PREFFTH";

ImageMetadata::ImageMetadata()
{
}

void ImageMetadata::setEntries(const std::map<QString,ImageMetadata::Entry>& map)
{
  entries = map;
}

const std::map<QString,ImageMetadata::Entry>& ImageMetadata::getEntries() const
{
  return entries;
}

void ImageMetadata::addEntry(const QString& key, const QString& value, const QString& comment)
{
  Entry entry{value,comment};
  entries[key] = entry;
}

QString ImageMetadata::getValue(const QString& key) const
{
  if (entries.find(key) == entries.end()) return "";
  return entries.at(key).value;
}

QString ImageMetadata::getComment(const QString& key) const
{
  if (entries.find(key) == entries.end()) return "";
  return entries.at(key).comment;
}

void ImageMetadata::setHistory(const QStringList& h)
{
  history = h;
  if (!history.empty() && history.back().trimmed().isEmpty())
  {
    history.pop_back();
  }
}

const QStringList& ImageMetadata::getHistory() const
{
  return history;
}

void ImageMetadata::addHistory(const QString& msg)
{
  history.push_back(msg);
}

void ImageMetadata::setObject(const QString& s)
{
  addEntry(keyObject,s);
}

QString ImageMetadata::getObject() const
{
  return getValue(keyObject);
}

void ImageMetadata::setObserver(const QString& s)
{
  addEntry(keyObserver,s);
}

QString ImageMetadata::getObserver() const
{
  return getValue(keyObserver);
}

void ImageMetadata::setInstrument(const QString& s)
{
  addEntry(keyInstrument,s);
}

QString ImageMetadata::getInstrument() const
{
  return getValue(keyInstrument);
}

void ImageMetadata::setTelescope(const QString& s)
{
  addEntry(keyTelescope,s);
}

QString ImageMetadata::getTelescope() const
{
  return getValue(keyTelescope);
}

void ImageMetadata::setExposureTime(double t)
{
  addEntry(keyExpTime,QString::number(t));
}

double ImageMetadata::getExposureTime() const
{
  return getValue(keyExpTime).toDouble();
}

void ImageMetadata::setObsDateTime(QDateTime t)
{
  addEntry(keyDateObs,t.date().toString(Qt::ISODate));
  addEntry(keyTimeObs,t.time().toString(Qt::ISODate));
}

QDateTime ImageMetadata::getObsDateTime() const
{
  QString date = getValue(keyDateObs);
  QString time = getValue(keyTimeObs);
  if (!date.isEmpty() && !time.isEmpty())
  {
    QDate d = QDate::fromString(date,Qt::ISODate);
    QTime t = QTime::fromString(time,Qt::ISODate);
    return QDateTime(d,t,Qt::UTC);
  }
  return QDateTime();
}

