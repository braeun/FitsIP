/********************************************************************************
 *                                                                              *
 * FitsIP - generic settings                                                    *
 *                                                                              *
 * modified: 2022-12-02                                                         *
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
#include "io/fitsio.h"
#include <QApplication>
#include <QFileDialog>
#include <QSettings>

const char* Settings::PATH_IMAGE = "fits/path/image";

const char* Settings::PATH_EXPORT = "fits/path/export";

const char* Settings::PATH_LOGBOOK = "fits/path/logbook";

static const char* PREVIEW_WIDTH = "fits/preview/width";
static const char* PREVIEW_HEIGHT = "fits/preview/height";

static const char* IO_ALWAYS_FITS = "fits/io/alwaysfits";
static const char* IO_METADATA_FILE = "fits/io/metadatafile";
static const char* IO_FITS_IMGFORMAT = "fits/io/fitsimageformat";

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
