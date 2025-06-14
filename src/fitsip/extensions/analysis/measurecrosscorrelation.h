/********************************************************************************
 *                                                                              *
 * FitsIP - perform cross carrelation                                           *
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

#ifndef MEASURECROSSCORRELATION_H
#define MEASURECROSSCORRELATION_H

#include <fitsip/core/opplugin.h>
#include <fitsip/core/dialogs/imageselectdialog.h>
#include <QObject>

/**
 * @brief The MeasureCrossCorrelation class performs a cross correlation between two images.
 *
 * The cross correlation is done in the frequency domain. The FFT of the first
 * image is multiplied with the complex conjugated FFT of the second image.
 * The result is transformed back into a real image.
 */
class MeasureCrossCorrelation: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  MeasureCrossCorrelation();
  virtual ~MeasureCrossCorrelation() override;

  virtual bool createsNewImage() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

  std::shared_ptr<FitsObject> correlate(std::shared_ptr<FitsObject> image1, std::shared_ptr<FitsObject> image2, QRect selection=QRect()) const;

private:
  void sort(double *s, double *d, int32_t w, int32_t h) const;

  std::shared_ptr<FitsObject> img;
  ImageSelectDialog* dlg;
};

#endif // MEASURECROSSCORRELATION_H
