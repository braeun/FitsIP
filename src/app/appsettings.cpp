/********************************************************************************
 *                                                                              *
 * FitsIP - application settings                                                *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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
const char* AppSettings::PATH_PIXELLIST = "fits/path/pixellist";
const char* AppSettings::PATH_STARLIST = "fits/path/starlist";
const char* AppSettings::PATH_PROFILE = "fits/path/profile";
const char* AppSettings::PATH_LAST = "fits/path/last";

static const char* FILE_FILTERS = "files/filter";
static const char* FILE_CURRENT_FILTER = "files/currentfilter";

static const char* FILELIST_ADD_ON_COPY = "files/addoncopy";

static const char* LOGBOOK_NAME = "logbook/name";
static const char* LOGBOOK_STYLE = "logbook/style";
static const char* LOGBOOK_LOGOPEN = "logbook/logopen";
static const char* LOGBOOK_OPENLAST = "logbook/openlast";

static const char* DISPLAY_SHOWPIXELLIST = "display/showpixellist";
static const char* DISPLAY_SHOWSTARLIST = "display/showstarlist";
static const char* DISPLAY_IMAGE_SCALE_FORALL = "display/scaleforall";

static const char* STYLE_STYLE = "style/style";
static const char* STYLE_PALETTE = "style/palette";

static const char* PROFILE_CLICK_STOPS_TRACK = "profile/clickstops";

static const char* SCRIPT_OUTPUT_LOG_WIDGET = "script/outputlogwidget";


AppSettings::AppSettings()
{
}

void AppSettings::setFileFilters(const QStringList& filters)
{
  settings.setValue(FILE_FILTERS,filters);
}

QStringList AppSettings::getFileFilters() const
{
  return settings.value(FILE_FILTERS,QStringList()).toStringList();
}

void AppSettings::setCurrentFileFilter(const QString& filter)
{
  settings.setValue(FILE_CURRENT_FILTER,filter);
}

QString AppSettings::getCurrentFileFilter() const
{
  return settings.value(FILE_CURRENT_FILTER,"*").toString();
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

void AppSettings::setImageScaleForAll(bool flag)
{
  settings.setValue(DISPLAY_IMAGE_SCALE_FORALL,flag);
}

bool AppSettings::isImageScaleForAll() const
{
  return settings.value(DISPLAY_IMAGE_SCALE_FORALL).toBool();
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

void AppSettings::setProfileStopTracking(bool flag)
{
  settings.setValue(PROFILE_CLICK_STOPS_TRACK,flag);
}

bool AppSettings::isProfileStopTracking() const
{
  return settings.value(PROFILE_CLICK_STOPS_TRACK,true).toBool();
}

void AppSettings::setScriptOutputToLogwidget(bool flag)
{
  settings.setValue(SCRIPT_OUTPUT_LOG_WIDGET,flag);
}

bool AppSettings::isScriptOutputToLogwidget() const
{
  return settings.value(SCRIPT_OUTPUT_LOG_WIDGET,false).toBool();
}

