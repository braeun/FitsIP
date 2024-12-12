/********************************************************************************
 *                                                                              *
 * FitsIP - vanCittert deconvolution                                            *
 *                                                                              *
 * modified: 2023-02-05                                                         *
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

#include "vancittertdeconvolution.h"
#include "vancittertdeconvolutiondialog.h"
#include "opcut.h"
#include <fitsbase/imagestatistics.h>
#include <fitsbase/fitsimage.h>
#include <fitsbase/dialogs/progressdialog.h>
#include <fitsbase/psf/psf.h>
#include <fitsbase/psf/psffactory.h>
#include <QApplication>
#include <QDebug>

VanCittertDeconvolution::VanCittertDeconvolution():
  cutImage(false),
  func(Constant),
  parameter(1),
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("VanCittertDeconvolution");
}

VanCittertDeconvolution::~VanCittertDeconvolution()
{
}

QString VanCittertDeconvolution::getMenuEntry() const
{
  return "Filter/van Cittert Deconvolution...";
}

QIcon VanCittertDeconvolution::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/vc.png");
}

OpPlugin::ResultType VanCittertDeconvolution::execute(std::shared_ptr<FitsImage> image, QRect selection)
{
  if (!dlg) dlg = new VanCittertDeconvolutionDialog();
  if (dlg->exec())
  {
    const PSF* psf = PSFFactory::getInstance()->getPSF(dlg->getFunction());
    if (psf)
    {
      cutImage = dlg->isCutImage();
      if (dlg->isSineFunction())
        func = Sine;
      else
        func = Constant;
      parameter = dlg->getParameter();
      auto psfpar = dlg->getParameters();
      profiler.start();
      deconvolve(image,psf,psfpar,dlg->getIterationCount(),true);
      profiler.stop();
      QString msg = "van Cittert deconvolution: ";
      msg += psf->getName() + " par=";
      for (size_t i=0;i<psfpar.size();++i)
      {
        if (i > 0) msg += ",";
        msg += QString::asprintf("%.1f",psfpar[i]);
      }
      msg += QString::asprintf("  niter=%d",dlg->getIterationCount());
      if (cutImage) msg += "  cut image";
      switch (func)
      {
        case Constant:
          msg += QString::asprintf("  relaxation=%.2f",parameter);
          break;
        case Sine:
          msg += QString::asprintf("  sine relaxation=%.2f",parameter);
          break;
      }
      log(image,msg);
      logProfiler(image);
      return OK;
    }
    else
    {
      setError("Internal Error: PSF '"+dlg->getFunction()+"' not found!");
      return ERROR;
    }
    return OK;
  }
  return CANCELLED;
}

void VanCittertDeconvolution::deconvolve(std::shared_ptr<FitsImage> image, const PSF* psf, const std::vector<ValueType>& par, int niter, bool progress)
{
  ProgressDialog* prog = progress && (niter > 2) ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(niter);
    prog->show();
  }
  ImageStatistics basestat(*image);
  auto o = std::make_shared<FitsImage>(*image);
  auto h = psf->createPSF(image->getWidth(),image->getHeight(),par);
  fftwidth = image->getWidth() / 2 + 1;
  fftheight = image->getHeight();
  fftsize = fftwidth * fftheight;
  fftw_complex* hfft = fft(*h,0);
  ImageStatistics stat;
  std::shared_ptr<FitsImage> c;
  while (niter-- > 0)
  {
    if (image->getDepth() == 1)
    {
      fftw_complex* offt = fft(*o,0);
      mul(offt,hfft,fftsize);
      c = invfft(offt,image->getWidth(),image->getHeight());
      delete [] offt;
    }
    else if (image->getDepth() == 3)
    {
      fftw_complex* offt1 = fft(*o,0);
      fftw_complex* offt2 = fft(*o,1);
      fftw_complex* offt3 = fft(*o,2);
      mul(offt1,hfft,fftsize);
      mul(offt2,hfft,fftsize);
      mul(offt3,hfft,fftsize);
      c = invfft(offt1,offt2,offt3,image->getWidth(),image->getHeight());
      delete [] offt1;
      delete [] offt2;
      delete [] offt3;
    }
    auto s = std::make_shared<FitsImage>(*image);
    *s -= *c;
    stat = ImageStatistics(*s);
    switch (func)
    {
      case Constant:
        *s *= parameter;
        break;
      case Sine:
        applySineRelaxation(o,basestat,s);
        break;
    }
    *o += *s;
    if (cutImage)
    {
      OpCut().cut(o,basestat.getGlobalStatistics().minValue,basestat.getGlobalStatistics().maxValue);
    }
    qInfo() << "remaining" << niter << " stddev=" << stat.getGlobalStatistics().stddev;
    if (prog)
    {
      prog->setProgress(niter);
      prog->appendMessage(QString::asprintf("stddev=%f",stat.getGlobalStatistics().stddev));
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
  }
  delete [] hfft;
  *image = *o;
  if (prog) prog->deleteLater();
}




fftw_complex* VanCittertDeconvolution::fft(const FitsImage &image, int channel)
{
  fftw_complex *s2c = new fftw_complex[fftsize];
  double *in = new double[image.getHeight()*image.getWidth()];
  fftw_plan f = fftw_plan_dft_r2c_2d(image.getHeight(),image.getWidth(),in,s2c,FFTW_ESTIMATE);
  ConstPixelIterator it = image.getConstPixelIterator();
  double* ptr = in;
  for (uint32_t i=0;i<image.getHeight()*image.getWidth();i++)
  {
    *ptr++ = it[channel];
    ++it;
  }
  fftw_execute(f);
  fftw_destroy_plan(f);
  delete [] in;
  return s2c;
}

std::shared_ptr<FitsImage> VanCittertDeconvolution::invfft(fftw_complex* c, int w, int h)
{
  fftw_complex* in = c;
  double *out = new double[w*h];
  fftw_plan f = fftw_plan_dft_c2r_2d(h,w,in,out,FFTW_ESTIMATE);
  fftw_execute(f);
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = out;
  for (uint32_t i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
  {
    it2[0] = *ptr;
    ++ptr;
    ++it2;
  }
  fftw_destroy_plan(f);
  delete [] out;
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  return fftimg;
}

std::shared_ptr<FitsImage> VanCittertDeconvolution::invfft(fftw_complex* c1, fftw_complex* c2, fftw_complex* c3, int w, int h)
{
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,3);
  double *out = new double[w*h];
  {
    fftw_plan f = fftw_plan_dft_c2r_2d(h,w,c1,out,FFTW_ESTIMATE);
    fftw_execute(f);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = out;
    for (uint32_t i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[0] = *ptr;
      ++ptr;
      ++it2;
    }
    fftw_destroy_plan(f);
  }
  {
    fftw_plan f = fftw_plan_dft_c2r_2d(h,w,c2,out,FFTW_ESTIMATE);
    fftw_execute(f);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = out;
    for (uint32_t i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[1] = *ptr;
      ++ptr;
      ++it2;
    }
    fftw_destroy_plan(f);
  }
  {
    fftw_plan f = fftw_plan_dft_c2r_2d(h,w,c3,out,FFTW_ESTIMATE);
    fftw_execute(f);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = out;
    for (uint32_t i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[2] = *ptr;
      ++ptr;
      ++it2;
    }
    fftw_destroy_plan(f);
  }
  delete [] out;
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  return fftimg;
}


void VanCittertDeconvolution::mul(fftw_complex *a, fftw_complex *b, int n)
{
  while (n-- > 0)
  {
    double re = (*a)[0] * (*b)[0] - (*a)[1] * (*b)[1];
    double im = (*a)[0] * (*b)[1] + (*a)[1] * (*b)[0];
    (*a)[0] = re;
    (*a)[1] = im;
    ++a;
    ++b;
  }
}

void VanCittertDeconvolution::applySineRelaxation(std::shared_ptr<FitsImage> image, const ImageStatistics& stat, std::shared_ptr<FitsImage> corr)
{
  ImageStatistics s = stat;
  if (!cutImage) s = ImageStatistics(*image);
  ConstPixelIterator p = image->getConstPixelIterator();
  PixelIterator c = corr->getPixelIterator();
  while (p.hasNext())
  {
    for (uint32_t i=0;i<image->getDepth();i++)
    {
      ValueType w;
      if (p[i] <= s.getLayerStatistics()[i].minValue)
        w = 0;
      else if (p[i] >= s.getLayerStatistics()[i].maxValue)
        w = 1;
      else
      {
        w = sin(M_PI_2*(p[i]-s.getLayerStatistics()[i].minValue)/(s.getLayerStatistics()[i].maxValue-s.getLayerStatistics()[i].minValue));
        w = std::pow(w,parameter);
      }
      c[i] *= w;
    }
    ++p;
    ++c;
  }
}
