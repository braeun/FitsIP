/********************************************************************************
 *                                                                              *
 * FitsIP - measure the sharpness of images                                     *
 *                                                                              *
 * modified: 2025-02-21                                                         *
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

#include <fitsip/core/opplugin.h>
#include <fitsip/core/dialogs/imageselectdialog.h>
#include <QObject>
#include <vector>

class MeasureSharpnessResultDialog;

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
 * The sharpness value is determined by applying the Laplacian kernel to the
 * image and than calculating the overall standard deviation.
 */
class MeasureSharpness: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  MeasureSharpness();
  virtual ~MeasureSharpness() override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(const std::vector<QFileInfo>& list, const OpPluginData& data=OpPluginData()) override;

  virtual ResultType execute(const std::vector<std::shared_ptr<FitsObject>>& list, const OpPluginData& data=OpPluginData()) override;

private:
  SharpnessData evaluate(const QFileInfo info, QRect selection) const;
  SharpnessData calculateSharpness(FitsImage* img, QRect selection=QRect()) const;
  void copyToLog();

  MeasureSharpnessResultDialog* resultDialog;
  std::vector<SharpnessData> results;

};

#endif // MEASURESHARPNESS_H
