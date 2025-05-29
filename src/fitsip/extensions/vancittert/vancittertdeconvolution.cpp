/********************************************************************************
 *                                                                              *
 * FitsIP - vanCittert deconvolution                                            *
 *                                                                              *
 * modified: 2025-05-28                                                         *
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
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/psf/psf.h>
#include <fitsip/core/psf/psffactory.h>
#include <QApplication>
#include <QDebug>

VanCittertDeconvolution::VanCittertDeconvolution():
  cutImage(false),
  func(Constant),
  parameter(1),
  dlg(nullptr)
{
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

OpPlugin::ResultType VanCittertDeconvolution::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new VanCittertDeconvolutionDialog();
  dlg->updatePSFList();
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
      deconvolve(image->getImage(),psf,psfpar,dlg->getIterationCount(),true);
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
  cinout = new fftw_complex[fftsize];
  rinout = new double[image->getHeight()*image->getWidth()];
  r2c = fftw_plan_dft_r2c_2d(image->getHeight(),image->getWidth(),rinout,cinout,FFTW_ESTIMATE);
  c2r = fftw_plan_dft_c2r_2d(image->getHeight(),image->getWidth(),cinout,rinout,FFTW_ESTIMATE);
  fft(*h,0);
  fftw_complex* hfft = new fftw_complex[fftsize];
  memcpy(hfft,cinout,fftsize*sizeof(fftw_complex));
  fftw_complex* offt1 = new fftw_complex[fftsize];
  fftw_complex* offt2 = new fftw_complex[fftsize];
  fftw_complex* offt3 = new fftw_complex[fftsize];
  ImageStatistics stat;
  std::shared_ptr<FitsImage> c;
  while (niter-- > 0)
  {
    if (image->getDepth() == 1)
    {
      fft(*o,0);
      memcpy(offt1,cinout,fftsize*sizeof(fftw_complex));
      mul(offt1,hfft,fftsize);
      c = invfft(offt1,image->getWidth(),image->getHeight());
    }
    else if (image->getDepth() == 3)
    {
      fft(*o,0);
      memcpy(offt1,cinout,fftsize*sizeof(fftw_complex));
      fft(*o,1);
      memcpy(offt2,cinout,fftsize*sizeof(fftw_complex));
      fft(*o,2);
      memcpy(offt3,cinout,fftsize*sizeof(fftw_complex));
      mul(offt1,hfft,fftsize);
      mul(offt2,hfft,fftsize);
      mul(offt3,hfft,fftsize);
      c = invfft(offt1,offt2,offt3,image->getWidth(),image->getHeight());
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
      o->cut(basestat.getGlobalStatistics().minValue,basestat.getGlobalStatistics().maxValue);
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
  fftw_destroy_plan(r2c);
  fftw_destroy_plan(c2r);
  delete [] rinout;
  delete [] cinout;
  delete [] offt1;
  delete [] offt2;
  delete [] offt3;
  delete [] hfft;
  *image = *o;
  if (prog) prog->deleteLater();
}




fftw_complex* VanCittertDeconvolution::fft(const FitsImage &image, int channel)
{
  ConstPixelIterator it = image.getConstPixelIterator();
  double* ptr = rinout;
  for (int i=0;i<image.getHeight()*image.getWidth();i++)
  {
    *ptr++ = it[channel];
    ++it;
  }
  fftw_execute(r2c);
  return cinout;
}

std::shared_ptr<FitsImage> VanCittertDeconvolution::invfft(fftw_complex* c, int w, int h)
{
  memmove(cinout,c,fftsize*sizeof(fftw_complex));
  fftw_execute(c2r);
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = rinout;
  for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
  {
    it2[0] = *ptr;
    ++ptr;
    ++it2;
  }
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  return fftimg;
}

std::shared_ptr<FitsImage> VanCittertDeconvolution::invfft(fftw_complex* c1, fftw_complex* c2, fftw_complex* c3, int w, int h)
{
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,3);
  {
    memmove(cinout,c1,fftsize*sizeof(fftw_complex));
    fftw_execute(c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[0] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  {
    memmove(cinout,c2,fftsize*sizeof(fftw_complex));
    fftw_execute(c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[1] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  {
    memmove(cinout,c3,fftsize*sizeof(fftw_complex));
    fftw_execute(c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[2] = *ptr;
      ++ptr;
      ++it2;
    }
  }
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
    for (int i=0;i<image->getDepth();i++)
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
