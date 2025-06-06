/********************************************************************************
 *                                                                              *
 * FitsIP - vanCittert deconvolution                                            *
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

#include "vancittertdeconvolution.h"
#include "vancittertdeconvolutiondialog.h"
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/io/iofactory.h>
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
  return QIcon(":/vancittert/resources/icons/vc.png");
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
      deconvolve(image->getImage(),psf,psfpar,dlg->getIterationCount(),true,dlg->isStoreIntermediate());
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

void VanCittertDeconvolution::deconvolve(std::shared_ptr<FitsImage> image, const PSF* psf, const std::vector<ValueType>& par,
                                         int niter, bool progress, bool storeintermediate, QString path)
{
  ProgressDialog* prog = progress && (niter > 2) ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(niter);
    prog->show();
  }
  int w0 = image->getWidth() + psf->getWidth();
  w0 += w0 % 2;
  int h0 = image->getHeight() + psf->getHeight();
  ImageStatistics basestat(*image);
  auto imgpadded = image->paddedImage(w0,h0); //std::make_shared<FitsImage>(*image);
  auto o = std::make_shared<FitsImage>(*imgpadded);
  auto h = psf->createPSF(w0,h0,par);
  fftdata data;
  data.fftsize = (w0 / 2 + 1) * h0;
  data.cinout = new fftw_complex[data.fftsize];
  data.rinout = new double[h0*w0];
  data.r2c = fftw_plan_dft_r2c_2d(h0,w0,data.rinout,data.cinout,FFTW_ESTIMATE);
  data.c2r = fftw_plan_dft_c2r_2d(h0,w0,data.cinout,data.rinout,FFTW_ESTIMATE);
  fft(data,*h,0);
  fftw_complex* hfft = new fftw_complex[data.fftsize];
  memcpy(hfft,data.cinout,data.fftsize*sizeof(fftw_complex));
  fftw_complex* offt1 = new fftw_complex[data.fftsize];
  fftw_complex* offt2 = new fftw_complex[data.fftsize];
  fftw_complex* offt3 = new fftw_complex[data.fftsize];
  ImageStatistics stat;
  std::shared_ptr<FitsImage> c;
  int remain = niter;
  while (remain-- > 0)
  {
    if (imgpadded->getDepth() == 1)
    {
      fft(data,*o,0);
      memcpy(offt1,data.cinout,data.fftsize*sizeof(fftw_complex));
      mul(offt1,hfft,data.fftsize);
      c = invfft(data,offt1,w0,h0);
    }
    else if (imgpadded->getDepth() == 3)
    {
      fft(data,*o,0);
      memcpy(offt1,data.cinout,data.fftsize*sizeof(fftw_complex));
      fft(data,*o,1);
      memcpy(offt2,data.cinout,data.fftsize*sizeof(fftw_complex));
      fft(data,*o,2);
      memcpy(offt3,data.cinout,data.fftsize*sizeof(fftw_complex));
      mul(offt1,hfft,data.fftsize);
      mul(offt2,hfft,data.fftsize);
      mul(offt3,hfft,data.fftsize);
      c = invfft(data,offt1,offt2,offt3,w0,h0);
    }
    auto s = std::make_shared<FitsImage>(*imgpadded);
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
    if (storeintermediate)
    {
      QString fnc = path + "/c_" + QString::number(niter-remain) + ".fts";
      IOHandler *io = IOFactory::getInstance()->getHandler(fnc);
      io->write(fnc,c);
      QString fns = path + "/s_" + QString::number(niter-remain) + ".fts";
      io->write(fns,s);
      QString fno = path + "/o_" + QString::number(niter-remain) + ".fts";
      io->write(fno,o);
    }
    qInfo() << "remaining" << remain << " stddev=" << stat.getGlobalStatistics().stddev;
    if (prog)
    {
      prog->setProgress(remain);
      prog->appendMessage(QString::asprintf("stddev=%f",stat.getGlobalStatistics().stddev));
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
  }
  fftw_destroy_plan(data.r2c);
  fftw_destroy_plan(data.c2r);
  delete [] data.rinout;
  delete [] data.cinout;
  delete [] offt1;
  delete [] offt2;
  delete [] offt3;
  delete [] hfft;
  /* crop to original size */
  o = o->subImage(QRect(0,0,image->getWidth(),image->getHeight()));
  *image = *o;
  if (prog) prog->deleteLater();
}




void VanCittertDeconvolution::fft(const fftdata& data, const FitsImage &image, int channel)
{
  ConstPixelIterator it = image.getConstPixelIterator();
  double* ptr = data.rinout;
  for (int i=0;i<image.getHeight()*image.getWidth();i++)
  {
    *ptr++ = it[channel];
    ++it;
  }
  fftw_execute(data.r2c);
}

std::shared_ptr<FitsImage> VanCittertDeconvolution::invfft(const fftdata& data, fftw_complex* c, int w, int h)
{
  memmove(data.cinout,c,data.fftsize*sizeof(fftw_complex));
  fftw_execute(data.c2r);
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = data.rinout;
  for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
  {
    it2[0] = *ptr;
    ++ptr;
    ++it2;
  }
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  return fftimg;
}

std::shared_ptr<FitsImage> VanCittertDeconvolution::invfft(const fftdata& data, fftw_complex* c1, fftw_complex* c2, fftw_complex* c3, int w, int h)
{
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,3);
  {
    memmove(data.cinout,c1,data.fftsize*sizeof(fftw_complex));
    fftw_execute(data.c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = data.rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[0] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  {
    memmove(data.cinout,c2,data.fftsize*sizeof(fftw_complex));
    fftw_execute(data.c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = data.rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[1] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  {
    memmove(data.cinout,c3,data.fftsize*sizeof(fftw_complex));
    fftw_execute(data.c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = data.rinout;
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
