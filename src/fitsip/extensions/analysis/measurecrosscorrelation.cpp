/********************************************************************************
 *                                                                              *
 * FitsIP - perform cross carrelation                                           *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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

#include "measurecrosscorrelation.h"
#include <fitsip/core/imagecollection.h>
#include <fitsip/core/fitsobject.h>
#include <fftw3.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

MeasureCrossCorrelation::MeasureCrossCorrelation():
  dlg(nullptr)
{
  profiler = SimpleProfiler("MeasureCrossCorrelation");
}

MeasureCrossCorrelation::~MeasureCrossCorrelation()
{
}

bool MeasureCrossCorrelation::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsObject>> MeasureCrossCorrelation::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{img};
}

QString MeasureCrossCorrelation::getMenuEntry() const
{
  return "Analyse/Cross Correlation...";
}

#ifdef USE_PYTHON
void MeasureCrossCorrelation::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("crosscorrelation",[this](std::shared_ptr<FitsObject> obj1, std::shared_ptr<FitsObject> obj2){
    return correlate(obj1,obj2);
    obj1->getImage()->log("Cross correlation between: "+obj1->getImage()->getName()+" and "+obj2->getImage()->getName());
  },
  "Cross correlation between to images",py::arg("obj1"),py::arg("obj2"));
}
#endif

OpPlugin::ResultType MeasureCrossCorrelation::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (dlg == nullptr)
  {
    dlg = new ImageSelectDialog();
    dlg->setTitle("Cross Correlation");
    dlg->setImageCollection(data.imageCollection);
  }
  if (dlg->exec())
  {
    std::shared_ptr<FitsObject> file = dlg->getImage();
    profiler.start();
    img = correlate(image,file,data.aoi);
    profiler.stop();
    log(img,"Cross correlation between: "+image->getImage()->getName()+" and "+file->getImage()->getName());
    logProfiler(img);
    return OK;
  }
  return CANCELLED;
}

std::shared_ptr<FitsObject> MeasureCrossCorrelation::correlate(std::shared_ptr<FitsObject> image1, std::shared_ptr<FitsObject> image2, QRect selection) const
{
  auto i1 = image1->getImage();
  auto i2 = image2->getImage();
  QRect r1 = QRect(0,0,i1->getWidth(),i1->getHeight());
  QRect r2 = QRect(0,0,i2->getWidth(),i2->getHeight());
  r1 = r1.intersected(r2);
  if (!selection.isNull()) r1 = r1.intersected(selection);
  auto img1 = i1->subImage(r1);
  auto img2 = i2->subImage(r1);
  fftw_complex *s2c = new fftw_complex[img1->getHeight()*(img1->getWidth()/2+1)];
  double *in = new double[img1->getHeight()*img1->getWidth()];
  fftw_plan f = fftw_plan_dft_r2c_2d(img1->getHeight(),img1->getWidth(),in,s2c,FFTW_ESTIMATE);
  fftw_plan b = fftw_plan_dft_c2r_2d(img1->getHeight(),img1->getWidth(),s2c,in,FFTW_ESTIMATE);
  ConstPixelIterator it = img1->getConstPixelIterator();
  double* ptr = in;
  for (int i=0;i<img1->getHeight()*img1->getWidth();i++)
  {
    *ptr++ = it.getAbs();
    ++it;
  }
  fftw_execute(f);
  fftw_complex *c1 = new fftw_complex[img1->getHeight()*(img1->getWidth()/2+1)];
  memcpy(c1,s2c,img1->getHeight()*(img1->getWidth()/2+1)*sizeof(fftw_complex));
  it = img2->getConstPixelIterator();
  ptr = in;
  for (int i=0;i<img2->getHeight()*img2->getWidth();i++)
  {
    *ptr++ = it.getAbs();
    ++it;
  }
  fftw_execute(f);
  fftw_complex* pc1 = c1;
  fftw_complex* pc2 = s2c;
  double norm = img1->getHeight() * img1->getWidth();
  for (int i=0;i<img1->getHeight()*(img1->getWidth()/2+1);i++)
  {
    double re = (*pc1)[0] * (*pc2[0]) + (*pc1)[1] * (*pc2)[1];
    double im = -(*pc1)[0] * (*pc2[1]) + (*pc1)[1] * (*pc2)[0];
    (*pc1)[0] = re;
    (*pc1)[1] = im;
    ++pc2;
    ++pc1;
  }
  memcpy(s2c,c1,img1->getHeight()*(img1->getWidth()/2+1)*sizeof(fftw_complex));
  fftw_execute(b);
  double *sin = new double[img1->getHeight()*img1->getWidth()];
  sort(in,sin,img1->getWidth(),img1->getHeight());
  auto ccimg = std::make_shared<FitsImage>(img2->getName()+"_cc",img1->getWidth(),img1->getHeight());
  PixelIterator it2 = ccimg->getPixelIterator();
  double *rptr = sin;
  for (int i=0;i<img1->getHeight()*img1->getWidth();i++)
  {
    it2[0] = *rptr / norm;
    ++rptr;
    ++it2;
  }
  fftw_destroy_plan(b);
  fftw_destroy_plan(f);
  delete [] s2c;
  delete [] c1;
  delete [] sin;
  delete [] in;
  return std::make_shared<FitsObject>(ccimg);
}


/*
 * shift 0,0 into the center of the image
 */
void MeasureCrossCorrelation::sort(double *s, double *d, int32_t w, int32_t h) const
{
  double *p;
  for (int y=0;y<h;y++)
  {
    if (y < h / 2 + h % 2)
    {
      int ys = (h / 2 + y) * w;
      int xh = w / 2 + w % 2;
      p = s + ys + w / 2;
      for (int x=0;x<xh;x++) *d++ = *p++;
      p = s + ys;
      for (int x=xh;x<w;x++) *d++ = *p++;
      /*
      {
        if (x < w / 2 + w % 2)
        {
          *d++ = s[ys + w / 2 + x];
        }
        else
        {
          *d++ = s[ys + x - w / 2 - w % 2];
        }
      }
      */
    }
    else
    {
      int ys = (y - h / 2 - h % 2) * w;
      int xh = w / 2 + w % 2;
      p = s + ys + w / 2;
      for (int x=0;x<xh;x++) *d++ = *p++;
      p = s + ys;
      for (int x=xh;x<w;x++) *d++ = *p++;
      /*
      for (int x=0;x<w;x++)
      {
        if (x < w / 2 + w % 2)
        {
          *d++ = s[ys + w / 2 + x];
        }
        else
        {
          *d++ = s[ys + x - w / 2 - w % 2];
        }
      }
      */
    }
  }
}
