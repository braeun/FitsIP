/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for logbook data storage                         *
 *                                                                              *
 * modified: 2022-11-29                                                         *
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

#include "logbookstorage.h"
#include "logbookentry.h"

LogbookStorage::LogbookStorage()
{

}

void LogbookStorage::setId(LogbookEntry &entry, int64_t v)
{
  entry.setId(v);
}

void LogbookStorage::setProject(LogbookEntry& entry, const QString& p)
{
  entry.setProject(p);
}

void LogbookStorage::setStep(LogbookEntry& entry, const QString& s)
{
  entry.setStep(s);
}

