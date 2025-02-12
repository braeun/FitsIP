/********************************************************************************
 *                                                                              *
 * FitsIP - measure the sharpness of images                                     *
 *                                                                              *
 * modified: 2025-02-12                                                         *
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

#ifndef MEASURESHARPNESS_H
#define MEASURESHARPNESS_H

#include <fitsbase/opplugin.h>
#include <fitsbase/dialogs/imageselectdialog.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

struct SharpnessData
{
  QFileInfo info;
  std::string filename;
  double min = 0;
  double max = 0;
  double mean = 0;
  double variance = 0;
  double minPixel = 0;
  double maxPixel = 0;
  double normalizedVariance = 0;
};

/**
 * @brief This plugin ranks images by sharpness.
 *
 * The sharpness value is determines by applying the Laplacian kernel to the
 * image and than calculating the overall standard deviation.
 */
class MeasureSharpness: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  MeasureSharpness();
  virtual ~MeasureSharpness() override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(const std::vector<QFileInfo>& list, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  virtual ResultType execute(const std::vector<std::shared_ptr<FitsObject>>& list, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

private:
  SharpnessData evaluate(const QFileInfo info, QRect selection);
  SharpnessData calculateSharpness(std::shared_ptr<FitsImage> img, QRect selection=QRect()) const;

  void copyToLog();

  std::vector<SharpnessData> results;

};

#endif // MEASURESHARPNESS_H
