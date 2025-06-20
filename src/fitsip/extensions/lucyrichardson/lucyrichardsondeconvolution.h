/********************************************************************************
 *                                                                              *
 * FitsIP - Lucy Richardson deconvolution                                       *
 *                                                                              *
 * modified: 2025-06-06                                                         *
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

#ifndef LUCYRICHARDSONDECONVOLUTION_H
#define LUCYRICHARDSONDECONVOLUTION_H

#include <fitsip/core/fitstypes.h>
#include <fitsip/core/opplugin.h>
#include <QObject>
#include <vector>
#include <fftw3.h>

class ImageStatistics;
class PSF;
class LucyRichardsonDeconvolutionDialog;

/**
 * @brief Lucy Richardson deconvolution
 *
 * See R.Berry and J. Purnell, Handbook of Astronomical Image Processing, 2nd edition
 */
class LucyRichardsonDeconvolution: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid FILE "plugin.json")
  Q_INTERFACES(OpPlugin)

public:
  enum RelaxationFunction { Constant, Sine };

  LucyRichardsonDeconvolution();
  virtual ~LucyRichardsonDeconvolution();

  virtual QString getMenuEntry() const override;

  virtual QIcon getIcon() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

  void deconvolve(std::shared_ptr<FitsImage> image, const PSF* psf, const std::vector<ValueType>& par, int niter, bool progress=false, bool storeintermediate=false);

private:
  struct fftdata
  {
    int fftsize;
    double* rinout;
    fftw_complex* cinout;
    fftw_plan r2c;
    fftw_plan c2r;
  };

//  std::shared_ptr<FitsImage> createPSF(uint32_t w, uint32_t h, const PSF* psf, const std::vector<ValueType>& par) const;
  void fft(const fftdata& data, const FitsImage &image, int channel);
  std::shared_ptr<FitsImage> invfft(const fftdata& data, fftw_complex* c, int w, int h);
  std::shared_ptr<FitsImage> invfft(const fftdata& data, fftw_complex* c1, fftw_complex* c2, fftw_complex* c3, int w, int h);
  /* calculate a*b overwriting a */
  void mul(fftw_complex* a, fftw_complex* b, int n);
  void applySineRelaxation(std::shared_ptr<FitsImage> image, const ImageStatistics& stat, std::shared_ptr<FitsImage> corr);

  bool cutImage;
  RelaxationFunction func;
  ValueType parameter;
  LucyRichardsonDeconvolutionDialog* dlg;

};


#endif // LUCYRICHARDSONDECONVOLUTION_H
