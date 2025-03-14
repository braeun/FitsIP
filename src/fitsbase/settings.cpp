/********************************************************************************
 *                                                                              *
 * FitsIP - generic settings                                                    *
 *                                                                              *
 * modified: 2025-03-13                                                         *
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

#include "settings.h"
#include <QApplication>
#include <QFileDialog>
#include <QSettings>

const char* Settings::PATH_IMAGE = "fits/path/image";

const char* Settings::PATH_EXPORT = "fits/path/export";

const char* Settings::PATH_LOGBOOK = "fits/path/logbook";

const char* Settings::PATH_SCRIPT = "fits/path/script";

static const char* PATH_INTERNAL = "fits/path/internal";

static const char* PREVIEW_WIDTH = "fits/preview/width";
static const char* PREVIEW_HEIGHT = "fits/preview/height";

static const char* IO_ALWAYS_FITS = "fits/io/alwaysfits";
static const char* IO_METADATA_FILE = "fits/io/metadatafile";
static const char* IO_FITS_IMGFORMAT = "fits/io/fitsimageformat";

static const char* TOOL_FILE_MANAGER = "fits/tools/filemanager";
static const char* TOOL_SCRIPT_EDITOR = "fits/tools/scripteditor";
static const char* TOOL_TEXT_EDITOR = "fits/tools/texteditor";
static const char* TOOL_OFFICE_EDITOR = "fits/tools/officeeditor";

static const char* LOGBOOK_LATEST_FIRST = "fits/logbook/latestfirst";

static const char* DB_NAME = "fits/db/name";
static const char* DB_DRIVER = "fits/db/driver";
static const char* DB_HOST = "fits/db/host";
static const char* DB_USER = "fits/db/user";
static const char* DB_PASSWORD = "fits/db/password";

Settings::Settings()
{
}

QString Settings::getString(const QString &setting, const QString &def)
{
  return settings.value(setting,def).toString();
}

QString Settings::getOpenFilename(QWidget* parent, const QString &setting, const QString &filter)
{
  QString path = settings.value(setting,".").toString();
  QString fn = QFileDialog::getOpenFileName(parent,QApplication::applicationDisplayName(),path,filter);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(setting,info.filePath());
  }
  return fn;
}

QString Settings::getSaveFilename(QWidget* parent, const QString &setting, const QString &filter, QString* selectedFilter)
{
  QString path = settings.value(setting,".").toString();
  QString fn = QFileDialog::getSaveFileName(parent,QApplication::applicationDisplayName(),path,filter,selectedFilter);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    settings.setValue(setting,info.filePath());
  }
  return fn;
}

QString Settings::getExistingDirectory(QWidget *parent, const QString &setting)
{
  QString path = settings.value(setting,".").toString();
  QString dir = QFileDialog::getExistingDirectory(parent,QApplication::applicationDisplayName(),path);
  if (!dir.isEmpty()) settings.setValue(setting,dir);
  return dir;
}

void Settings::setPreviewWidth(int w)
{
  settings.setValue(PREVIEW_WIDTH,w);
}

int Settings::getPreviewWidth() const
{
  return settings.value(PREVIEW_WIDTH,200).toInt();
}

void Settings::setPreviewHeight(int h)
{
  settings.setValue(PREVIEW_HEIGHT,h);
}

int Settings::getPreviewHeight() const
{
  return settings.value(PREVIEW_HEIGHT,200).toInt();
}

void Settings::setAlwaysSaveFits(bool flag)
{
  settings.setValue(IO_ALWAYS_FITS,flag);
}

bool Settings::isAlwaysSaveFits() const
{
  return settings.value(IO_ALWAYS_FITS,false).toBool();
}

void Settings::setWriteMetadataFile(bool flag)
{
  settings.setValue(IO_METADATA_FILE,flag);
}

bool Settings::isWriteMetadataFile() const
{
  return settings.value(IO_METADATA_FILE,true).toBool();
}

void Settings::setFitsImageFormat(int fmt)
{
  settings.setValue(IO_FITS_IMGFORMAT,fmt);
}

int Settings::getFitsImageFormat() const
{
  return settings.value(IO_FITS_IMGFORMAT,0).toInt();
}

void Settings::setTool(Tools tool, QString cmd)
{
  switch (tool)
  {
    case FileManager:
      settings.setValue(TOOL_FILE_MANAGER,cmd);
      break;
    case ScriptEditor:
      settings.setValue(TOOL_SCRIPT_EDITOR,cmd);
      break;
    case TextEditor:
      settings.setValue(TOOL_TEXT_EDITOR,cmd);
      break;
    case OfficeEditor:
      settings.setValue(TOOL_OFFICE_EDITOR,cmd);
      break;
  }
}

QString Settings::getTool(Tools tool) const
{
  switch (tool)
  {
    case FileManager:
      return settings.value(TOOL_FILE_MANAGER,"").toString();
    case ScriptEditor:
      return settings.value(TOOL_SCRIPT_EDITOR,"").toString();
    case TextEditor:
      return settings.value(TOOL_TEXT_EDITOR,"").toString();
    case OfficeEditor:
      return settings.value(TOOL_OFFICE_EDITOR,"").toString();
  }
  return "";
}

void Settings::setLogbookLatestFirst(bool flag)
{
  settings.setValue(LOGBOOK_LATEST_FIRST,flag);
}

bool Settings::isLogbookLatestFirst() const
{
  return settings.value(LOGBOOK_LATEST_FIRST,false).toBool();
}

void Settings::setInternalDirectory(QString path)
{
  settings.setValue(PATH_INTERNAL,path);
}

QString Settings::getInternalDirectory() const
{
  QString path = settings.value(PATH_INTERNAL,"").toString();
  if (path.isEmpty())
  {
    path = QDir::home().absoluteFilePath(".fitsip");
  }
  QFileInfo info(path);
  if (info.exists())
  {
    if (!info.isDir())
    {
      QFile::remove(path);
    }
  }
  if (!info.exists())
  {
    if (!QDir(path).mkpath(path))
    {
      path = QDir::homePath();
    }
  }
  return path;
}

QString Settings::getInternalPSFDirectory() const
{
  QFileInfo info(getInternalDirectory()+"/psf");
  if (!info.exists())
  {
    QDir(getInternalDirectory()).mkpath("psf");
  }
  return info.absoluteFilePath();
}

void Settings::setDatabase(QString name)
{
  settings.setValue(DB_NAME,name);
}

QString Settings::getDatabase() const
{
  return settings.value(DB_NAME,"fitsdb").toString();
}

void Settings::setDatabaseDriver(QString name)
{
  settings.setValue(DB_DRIVER,name);
}

QString Settings::getDatabaseDriver() const
{
  return settings.value(DB_DRIVER,"QSQLITE").toString();
}

void Settings::setDatabaseHost(QString name)
{
  settings.setValue(DB_HOST,name);
}

QString Settings::getDatabaseHost() const
{
  return settings.value(DB_HOST,"").toString();
}

void Settings::setDatabaseUser(QString name)
{
  settings.setValue(DB_USER,name);
}

QString Settings::getDatabaseUser() const
{
  return settings.value(DB_USER,"").toString();
}

void Settings::setDatabasePassword(QString name)
{
  settings.setValue(DB_PASSWORD,name);
}

QString Settings::getDatabasePassword() const
{
  return settings.value(DB_PASSWORD,"").toString();
}

