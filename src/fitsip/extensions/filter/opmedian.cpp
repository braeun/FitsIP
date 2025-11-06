/********************************************************************************
 *                                                                              *
 * FitsIP - median filter                                                       *
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

#include "opmedian.h"
#include "opmediandialog.h"
#include <fitsip/core/fitsimage.h>
#include <algorithm>
#include <QApplication>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpMedian::OpMedian():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpMedian");
}

OpMedian::~OpMedian()
{
}

QString OpMedian::getMenuEntry() const
{
  return "Filter/Median...";
}

#ifdef USE_PYTHON
void OpMedian::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("median_filter",[this](std::shared_ptr<FitsObject> obj, int size, ValueType threshold){
    filter(&obj->getImage(),threshold,size);
    obj->getImage().log(QString::asprintf("Median filter: size=%d threshold=%f",size,threshold));
    return OK;
  },
  "Apply median filter",py::arg("obj"),py::arg("size"),py::arg("threshold"));
}
#endif

OpPlugin::ResultType OpMedian::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new OpMedianDialog();
  dlg->setSourceImage(image->getImage(),data.aoi,data.previewOptions);
  if (dlg->exec())
  {
    QApplication::setOverrideCursor(Qt::BusyCursor);
    ValueType threshold = dlg->getThreshold();
    int size = dlg->getSize();
    profiler.start();
    filter(&image->getImage(),threshold,size);
    profiler.stop();
    log(image,QString::asprintf("Median filter: size=%d threshold=%f",size,threshold));
    logProfiler(image);
    QApplication::restoreOverrideCursor();
    return OK;
  }
  return CANCELLED;
}

void OpMedian::filter(FitsImage* image, ValueType threshold, int size) const
{
  FitsImage tmp(*image);
//  PixelIterator it1 = img.getPixelIterator();
  int s2 = size / 2;
//  it1 += hk2 * img.getWidth();
  for (int y0=s2;y0<image->getHeight()-s2;y0++)
  {
//    it1 += wk2;
    for (int x0=s2;x0<image->getWidth()-s2;x0++)
    {
      std::vector<ValueType> neighbors[image->getDepth()];
      int y = y0 - s2;
      for (int32_t rk=0;rk<size;rk++)
      {
        ConstPixelIterator it2 = tmp.getConstPixelIterator(x0-s2,y);
        for (int32_t ck=0;ck<size;ck++)
        {
          for (int d=0;d<image->getDepth();d++)
          {
            neighbors[d].push_back(it2[d]);
          }
          ++it2;
        }
        y++;
      }
      PixelIterator it1 = image->getPixelIterator(x0,y0);
      for (int d=0;d<image->getDepth();d++)
      {
        std::sort(neighbors[d].begin(),neighbors[d].end());
        ValueType sigma = (3*neighbors[d][neighbors->size()/4] - neighbors[d][neighbors->size()/4]) / 2;
        if (it1[d] > neighbors[d][neighbors->size()/2] + threshold*sigma)
          it1[d] = neighbors[d][neighbors->size()/2];
        if (it1[d] < neighbors[d][neighbors->size()/2] - threshold*sigma)
          it1[d] = neighbors[d][neighbors->size()/2];
      }
//      ++it1;
    }
//    it1 += wk2;
  }
}


