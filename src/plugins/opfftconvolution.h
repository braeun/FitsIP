/********************************************************************************
 *                                                                              *
 * FitsIP - convolve an image using FFT                                         *
 *                                                                              *
 * modified: 2025-03-01                                                         *
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

#ifndef OPFFTCONVOLUTION_H
#define OPFFTCONVOLUTION_H

#include <fitsbase/opplugin.h>
#include <QObject>

#define QT_STATICPLUGIN
#include <QtPlugin>

class PSF;
class OpFFTConvolutionDialog;

class OpFFTConvolution: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)
public:
  OpFFTConvolution();
  virtual ~OpFFTConvolution() override;

  virtual QString getMenuEntry() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

  std::shared_ptr<FitsImage> fftconvolution(FitsImage* img, const PSF* psf, const std::vector<ValueType>& par) const;

private:
  OpFFTConvolutionDialog* dlg;

};

#endif // OPFFTCONVOLUTION_H
