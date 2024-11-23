/********************************************************************************
 *                                                                              *
 * FitsIP - application settings                                                *
 *                                                                              *
 * modified: 2022-12-04                                                         *
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

#include "appsettings.h"
#include <QSettings>

const char* AppSettings::MAINWINDOW_STATE = "mainwindow/state";
const char* AppSettings::MAINWINDOW_GEOMETRY = "mainwindow/geometry";

const char* AppSettings::PATH_LOG = "fits/path/log";
const char* AppSettings::PATH_ROOT = "fits/path/root";
const char* AppSettings::PATH_PLUGIN = "fits/path/plugin";
const char* AppSettings::PATH_FILELIST = "fits/path/filelist";

static const char* FILE_FILTER = "files/filter";

static const char* FILELIST_ADD_ON_COPY = "files/addoncopy";

static const char* LOGBOOK_NAME = "logbook/name";
static const char* LOGBOOK_STYLE = "logbook/style";
static const char* LOGBOOK_LOGOPEN = "logbook/logopen";
static const char* LOGBOOK_OPENLAST = "logbook/openlast";

static const char* DISPLAY_SHOWPIXELLIST = "display/showpixellist";
static const char* DISPLAY_SHOWSTARLIST = "display/showstarlist";

static const char* STYLE_STYLE = "style/style";
static const char* STYLE_PALETTE = "style/palette";


AppSettings::AppSettings()
{
}

void AppSettings::setFileFilters(const QStringList& filters)
{
  settings.setValue(FILE_FILTER,filters);
}

QStringList AppSettings::getFileFilters() const
{
  return settings.value(FILE_FILTER,QStringList()).toStringList();
}

bool AppSettings::isFilelistAddOnCopy() const
{
  return settings.value(FILELIST_ADD_ON_COPY,true).toBool();
}

void AppSettings::setLogbook(const QString &name)
{
  settings.setValue(LOGBOOK_NAME,name);
}

QString AppSettings::getLogbook() const
{
  return settings.value(LOGBOOK_NAME,"").toString();
}

bool AppSettings::isLogbookOpenLast() const
{
  return settings.value(LOGBOOK_OPENLAST,true).toBool();
}

void AppSettings::setLogbookOpenLast(bool flag)
{
  settings.setValue(LOGBOOK_OPENLAST,flag);
}

bool AppSettings::isLogbookLogOpen() const
{
  return settings.value(LOGBOOK_LOGOPEN,true).toBool();
}

void AppSettings::setLogbookLogOpen(bool flag)
{
  settings.setValue(LOGBOOK_LOGOPEN,flag);
}

void AppSettings::setLogwidgetStyle(int32_t style)
{
  settings.setValue(LOGBOOK_STYLE,style);
}

int32_t AppSettings::getLogwidgetStyle() const
{
  return settings.value(LOGBOOK_STYLE,0).toInt();
}

void AppSettings::setShowPixellist(bool flag)
{
  settings.setValue(DISPLAY_SHOWPIXELLIST,flag);
}

bool AppSettings::isShowPixellist() const
{
  return settings.value(DISPLAY_SHOWPIXELLIST).toBool();
}

void AppSettings::setShowStarlist(bool flag)
{
  settings.setValue(DISPLAY_SHOWSTARLIST,flag);
}

bool AppSettings::isShowStarlist() const
{
  return settings.value(DISPLAY_SHOWSTARLIST).toBool();
}

void AppSettings::setStyle(const QString &style)
{
  settings.setValue(STYLE_STYLE,style);
}

QString AppSettings::getStyle() const
{
  return settings.value(STYLE_STYLE,"fusion").toString();
}

void AppSettings::setPalette(const QString &p)
{
  settings.setValue(STYLE_PALETTE,p);
}

QString AppSettings::getPalette() const
{
  return settings.value(STYLE_PALETTE).toString();
}

