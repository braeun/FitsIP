/********************************************************************************
 *                                                                              *
 * FitsIP - base class for operation plugins                                    *
 *                                                                              *
 * modified: 2025-01-10                                                         *
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

#ifndef OPPLUGIN_H
#define OPPLUGIN_H

#include "plugin.h"
#include "fitsobject.h"
#include "profiling/simpleprofiler.h"
#include "widgets/previewoptions.h"
#include <QFileInfo>
#include <QLoggingCategory>
#include <QRect>
#include <vector>
#include <memory>

Q_DECLARE_LOGGING_CATEGORY(LOG_PROFILER)

class OpPlugin: public Plugin
{
  Q_OBJECT
public:

  enum ResultType { OK=0, CANCELLED, ERROR };

  enum class SourceType { NONE=0, ACTIVE_IMAGE, FILELIST };

  OpPlugin(bool interactive=true);
  virtual ~OpPlugin() override;

  virtual bool requiresImage() const;

  virtual bool requiresFileList() const;

  /**
   * @brief Returns whether the plugin creates a new fits image.
   * @return true if a new image is created
   */
  virtual bool createsNewImage() const;

  /**
   * @brief Return the newly created images.
   *
   * All plugins which do not create a new image will return an empty list
   * here.
   * @return list of created images
   */
  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions());

  virtual ResultType execute(const std::vector<QFileInfo>& list, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions());

  /**
   * @brief Get the file list generated by this plugin.
   *
   * Some plugins might generate a list of files. This list can be retrieved
   * with this method. Any plugin not generating such a list must return an empty
   * list here.
   * @return the file list
   */
  const std::vector<QFileInfo> getFileList() const;

  virtual QString getError() const;

signals:
  void logOperation(QString image, QString op);

  void logProfilerResult(QString profiler, QString image, int w, int h, int64_t t, QString notes);

protected:
  void setError(const QString& err);

  /**
   * @brief Save the image to disk
   * @param image the image to save
   * @param outputpath the path where to save the image
   * @param info the original file info (needed for filename)
   * @param tag optional tag, which will be appended to the basename of the file
   * @return result of the operation
   */
  virtual ResultType save(std::shared_ptr<FitsImage> image, const QString& outputpath, const QFileInfo& info, const QString& tag="");

  /**
   * @brief Save the image to disk
   * @param image the image to save
   * @param outputpath the path where to save the image
   * @param info the original file info (needed for filename)
   * @param tag optional tag, which will be appended to the basename of the file
   * @return result of the operation
   */
  virtual ResultType save(std::shared_ptr<FitsObject> image, const QString& outputpath, const QFileInfo& info, const QString& tag="");

  void log(std::shared_ptr<FitsImage> image, const QString& msg);

  void log(std::shared_ptr<FitsObject> image, const QString& msg);

  void logProfiler(const QString& image, const QString& msg="");

  void logProfiler(std::shared_ptr<FitsImage> image, const QString& msg="");

  void logProfiler(std::shared_ptr<FitsObject> image, const QString& msg="");

  bool interactive;
  std::vector<QFileInfo> filelist;
  QString error;
  SimpleProfiler profiler;

};

#define OpPlugin_iid "net.hbr.OpPlugin/1.0"

Q_DECLARE_INTERFACE(OpPlugin,OpPlugin_iid);


#endif // OPPLUGIN_H
