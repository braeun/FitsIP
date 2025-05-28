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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <cstdint>

class QWidget;

/**
 * @brief Class containing generic settings.
 *
 * Settings in this class are available to the base library and all plugins.
 */
class Settings
{
public:

  enum Tools {
    FileManager, ScriptEditor, TextEditor, OfficeEditor
  };

  Settings();

  QString getString(const QString& setting, const QString& def=QString());

  QString getOpenFilename(QWidget* parent, const QString& setting, const QString& filter=QString());

  QString getSaveFilename(QWidget* parent, const QString& setting, const QString& filter=QString(), QString* selectedFilter=nullptr);

  QString getExistingDirectory(QWidget* parent, const QString& setting);

  /**
   * @brief Set the default width for previews in plugins.
   * @param w the width
   */
  void setPreviewWidth(int w);

  /**
   * @brief Get the default width for previews in plugins.
   * @return the width
   */
  int getPreviewWidth() const;

  /**
   * @brief Set the default height for previews in plugins.
   * @param h the height
   */
  void setPreviewHeight(int h);

  /**
   * @brief Get the default height for previews in plugins.
   * @return the height
   */
  int getPreviewHeight() const;

  /**
   * @brief Set if plugins should always save in Fits format.
   * @param flag true if saving is always in Fits format
   */
  void setAlwaysSaveFits(bool flag);

  /**
   * @brief Get if plugins should always save in Fits format.
   * @return true if saving is always in Fits format
   */
  bool isAlwaysSaveFits() const;

  /**
   * @brief Set if images saved in formats other than Fits format should have
   * an extra file with metadata written when saving.
   * @param flag true if an extra file with metadata should be written
   */
  void setWriteMetadataFile(bool flag);

  /**
   * @brief Get if images saved in formats other than Fits format should have
   * an extra file with metadata written when saving.
   * @return true if an extra file with metadata should be written
   */
  bool isWriteMetadataFile() const;

  void setFitsImageFormat(int fmt);

  int getFitsImageFormat() const;

  void setTool(Tools tool, QString cmd);

  QString getTool(Tools tool) const;

  void setLogbookLatestFirst(bool flag);

  bool isLogbookLatestFirst() const;

  void setInternalDirectory(QString path);

  QString getInternalDirectory() const;

  QString getInternalPSFDirectory() const;

  void setDatabase(QString name);

  QString getDatabase() const;

  void setDatabaseDriver(QString name);

  QString getDatabaseDriver() const;

  void setDatabaseHost(QString name);

  QString getDatabaseHost() const;

  void setDatabaseUser(QString name);

  QString getDatabaseUser() const;

  void setDatabasePassword(QString name);

  QString getDatabasePassword() const;


  static const char* PATH_IMAGE;    //!< key for fits image path

  static const char* PATH_EXPORT;   //!< key for export image path

  static const char* PATH_LOGBOOK;  //!< key for logbook path

  static const char* PATH_SCRIPT;    //!< key for script path

protected:
  QSettings settings;

};

#endif // SETTINGS_H
