/********************************************************************************
 *                                                                              *
 * FitsIP - application settings                                                *
 *                                                                              *
 * modified: 2022-12-03                                                         *
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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <settings.h>
#include <QByteArray>

/**
 * @brief The AppSettings class extends the generic Settings class with settings
 *        for the graphical user interface only.
 */
class AppSettings: public Settings
{
public:
  AppSettings();

  void setFileFilter(QString filter);

  QString getFileFilter() const;

  bool isFilelistAddOnCopy() const;

  void setLogbook(const QString& name);

  QString getLogbook() const;

  bool isLogbookLogOpen() const;

  void setLogbookLogOpen(bool flag);

  void setLogwidgetStyle(int32_t style);

  int32_t getLogwidgetStyle() const;

  void setShowPixellist(bool flag);

  bool isShowPixellist() const;

  void setShowStarlist(bool flag);

  bool isShowStarlist() const;

  void setStyle(const QString& style);

  QString getStyle() const;

  void setPalette(const QString& p);

  QString getPalette() const;

  static const char* MAINWINDOW_STATE;
  static const char* MAINWINDOW_GEOMETRY;
  static const char* PATH_LOG;
  static const char* PATH_ROOT;
  static const char* PATH_PLUGIN;
  static const char* PATH_FILELIST;

};

#endif // SETTINGS_H
