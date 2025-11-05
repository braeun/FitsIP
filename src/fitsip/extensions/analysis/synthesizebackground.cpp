/********************************************************************************
 *                                                                              *
 * FitsIP - synthesize background from image                                    *
 *                                                                              *
 * modified: 2025-08-27                                                         *
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

#include "synthesizebackground.h"
#include "synthesizebackgrounddialog.h"
#include <fitsip/core/histogram.h>
#include <cmath>

std::mt19937 SynthesizeBackground::rng;

SynthesizeBackground::SynthesizeBackground():
  dlg(nullptr)
{
  profiler = SimpleProfiler("SynthesizeBackground");
}

SynthesizeBackground::~SynthesizeBackground()
{
  if (dlg) delete dlg;
}

QString SynthesizeBackground::getMenuEntry() const
{
  return "Analyse/Synthesize Background...";
}

std::vector<std::shared_ptr<FitsObject>> SynthesizeBackground::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}

OpPlugin::ResultType SynthesizeBackground::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new SynthesizeBackgroundDialog();
  Histogram hist;
  hist.build(*image->getImage());
  dlg->setSky(hist.getAverage(0.75));
  if (dlg->exec())
  {
    profiler.start();
    std::vector<Pixel> list;
    double bkg = dlg->getBackground();
    uint32_t n = dlg->getPointsCount();
    switch (dlg->getSelectionMode())
    {
      case 0:
        list = getGridPoints(image->getImage(),n,bkg);
        break;
      case 1:
        list = getRandomPoints(image->getImage(),n,bkg);
        break;
    }
    if (list.empty())
    {
      setError("No background points found");
      return ERROR;
    }
    uint32_t deg = dlg->getPolynomDegree();
    std::vector<std::vector<double>> coeff;
    for (int l=0;l<image->getImage()->getDepth();l++)
    {
      coeff.push_back(getCoefficients(list,deg,l));
    }
    img = createImage(image->getImage()->getWidth(),image->getImage()->getHeight(),coeff,deg);
    profiler.stop();
    log(img.get(),"Synthesized Background");
    logProfiler(img.get());
    return OK;
  }
  return CANCELLED;
}

std::vector<Pixel> SynthesizeBackground::getRandomPoints(FitsImage* image, uint32_t n, double bkg)
{
  std::vector<Pixel> list;
  std::uniform_int_distribution<std::mt19937::result_type> distW(0,image->getWidth()-1);
  std::uniform_int_distribution<std::mt19937::result_type> distH(0,image->getHeight()-1);
  while (--n > 0)
  {
    uint32_t x = distW(rng);
    uint32_t y = distH(rng);
    ConstPixelIterator it = image->getConstPixelIterator(x,y);
    if (it.getRGB().gray() <= bkg)
    {
      list.push_back(image->getPixel(it));
    }
  }
  return list;
}

std::vector<Pixel> SynthesizeBackground::getGridPoints(FitsImage* image, uint32_t n, double bkg)
{
  std::vector<Pixel> list;
  uint32_t nx = static_cast<uint32_t>(sqrt(n));
  uint32_t ny = n / nx;
  uint32_t dy = image->getHeight() / ny;
  uint32_t dx = image->getWidth() / nx;
  uint32_t yi = dy / 2;
  for (uint32_t j=0;j<ny;j++)
  {
    ConstPixelIterator it = image->getConstPixelIterator(dx/2,yi);
    for (uint32_t i=0;i<nx;i++)
    {
      if (it.getRGB().gray() <= bkg)
      {
        list.push_back(image->getPixel(it));
      }
      it += dx;
    }
    yi += dy;
  }
  return list;
}

std::vector<double> SynthesizeBackground::getCoefficients(const std::vector<Pixel>& pixels, uint32_t deg, uint32_t layer)
{
  uint32_t m,k,l;

  uint32_t dim = 1;
  for (uint32_t i=1;i<=deg;i++) dim += i + 1;
  uint32_t dims = 2 * deg + 1;
  double *sums = new double[dims*dims];
  memset(sums,0,dims*dims*sizeof(double));
  for (uint32_t j=0;j<dims;j++)
  {
    m = 0;
    k = j;
    l = 0;
    while (l <= j)
    {
      for (const Pixel& px : pixels)
      {
        sums[m*dims+j] += pow((double)px.x,k) * pow((double)px.y,l);
      }
      m++;
      k--;
      l++;
    }
  }
  uint32_t *rang1 = new uint32_t[dim];
  uint32_t *rang2 = new uint32_t[dim];
  rang1[0] = 0;
  rang2[0] = 0;
  m = 1;
  for (uint32_t i=1;i<=deg;i++)
  {
    k = i;
    l = 0;
    while (l < i+1)
    {
      rang1[m] = k;
      rang2[m] = l;
      m++;
      k--;
      l++;
    }
  }
  double *mat = new double[dim*dim];
  for (uint32_t i=0;i<dim;i++)
  {
    for (uint32_t j=0;j<dim;j++)
    {
      k = rang1[j] + rang1[i];
      l = rang2[j] + rang2[i];
      mat[i*dim+j] = sums[l*dims+k+l];
    }
  }
  double *yg = new double[dim];
  std::vector<double> xg(dim);
  uint32_t *cg = new uint32_t[dim];
  for (uint32_t i=0;i<dim;i++) yg[i] = 0.0;
  for (const Pixel& px : pixels) yg[0] += px.i[layer];
  m = 1;
  for (uint32_t i=1;i<=deg;i++)
  {
    k = i;
    l = 0;
    while (l < i+1)
    {
      for (const Pixel& px : pixels)
      {
        yg[m] += pow((double)px.x,(double)k) * pow((double)px.y,(double)l) * px.i[layer];
      }
      m++;
      k--;
      l++;
    }
  }
  for (uint32_t i=0;i<dim;i++) cg[i] = i;
  for (uint32_t k=0;k<dim-1;k++)
  {
    uint32_t n = k;
    for (uint32_t i=k+1;i<dim;i++) if (fabs(mat[k*dim+n])<fabs(mat[k*dim+i])) n = i;
    if (n != k)
    {
      for (uint32_t j=0;j<dim;j++)
      {
        double s = mat[j*dim+k];
        mat[j*dim+k] = mat[j*dim+n];
        mat[j*dim+n] = s;
      }
    }
    uint32_t si = cg[k];
    cg[k] = cg[n];
    cg[n] = si;

    double dp = 1.0 / mat[k*dim+k];
    for (uint32_t j=k;j<dim;j++) mat[k*dim+j] *= dp;
    yg[k] *= dp;
    for (uint32_t j=k+1;j<dim;j++)
    {
      double d = mat[j*dim+k];
      for (uint32_t l=k;l<dim;l++) mat[j*dim+l] -= d * mat[k*dim+l];
      yg[j] -= d * yg[k];
    }
  }
  xg[dim-1] = yg[dim-1] / mat[(dim-1)*dim+dim-1];
  for (uint32_t i=1;i<=dim-1;i++)
  {
    double s = 0;
    for (uint32_t j=dim-i+1;j<=dim;j++) s += mat[(dim-i-1)*dim+j-1] * xg[j-1];
    xg[dim-i-1] = yg[dim-i-1] - s;
  }
  for (uint32_t i=0;i<dim;i++) yg[cg[i]] = xg[i];
  for (uint32_t i=0;i<dim;i++) xg[i] = yg[i];
  delete [] sums;
  delete [] mat;
  delete [] rang1;
  delete [] rang2;
  delete [] cg;
  delete [] yg;
  return xg;
}

std::shared_ptr<FitsImage> SynthesizeBackground::createImage(u_int32_t w, uint32_t h, std::vector<std::vector<double>> coeff, uint32_t deg)
{
  std::shared_ptr<FitsImage> img = std::make_shared<FitsImage>("Background",w,h,coeff.size());
  PixelIterator it = img->getPixelIterator();
  for (int y=0;y<img->getHeight();y++)
  {
    for (int x=0;x<img->getWidth();x++)
    {
      for (size_t l=0;l<coeff.size();l++)
      {
        const std::vector<double>& xg = coeff[l];
        double s = xg[0];
        uint32_t m = 1;
        for (uint32_t o=0;o<deg;o++)
        {
          uint32_t k = o + 1;
          uint32_t l = 0;
          while (l<=o+1)
          {
            s += xg[m] * pow((double)x,(double)k) * pow((double)y,(double)l);
            m++;
            k--;
            l++;
          }
        }
        it[l] = s;
      }
      ++it;
    }
  }
  return img;
}

