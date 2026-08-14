/********************************************************************************
 *                                                                              *
 * FitsIP - crop image                                                          *
 *                                                                              *
 * modified: 2026-08-14                                                         *
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

#include "opcrop.h"
#include "opcropdialog.h"
#include <fitsip/core/fitsimage.h>
#include <algorithm>
#include <limits>
#include <QMessageBox>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpCrop::OpCrop():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpCrop");
}

OpCrop::~OpCrop()
{
  if (dlg) dlg->deleteLater();
}

QString OpCrop::getMenuEntry() const
{
  return "Image/Crop...";
}

QIcon OpCrop::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/transform-crop.png");
}

#ifdef USE_PYTHON
void OpCrop::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("crop",[](std::shared_ptr<FitsObject> obj, int x, int y, int w, int h){
      if (!obj) return ERROR;
      QRect r(x,y,w,h);
      if (!r.isValid()) return ERROR;
      auto img = obj->getImage().subImage(r);
      img.setMetadata(obj->getImage().getMetadata());
      obj->setImage(img);
      obj->getImage().log(QString("OpCrop: %1,%2 %3x%4").arg(x).arg(y).arg(w).arg(h));
      return OK;
    },
    "Crop the image",py::arg("obj"),py::arg("x"),py::arg("y"),py::arg("w"),py::arg("h"));
  m->def("autocrop",[this](std::shared_ptr<FitsObject> obj, ValueType threshold, int neighbors, int border){
      if (!obj) return ERROR;
      QRect r = findArea(obj->getImage(),threshold,neighbors,border);
      if (!r.isValid()) return ERROR;
      auto img = obj->getImage().subImage(r);
      img.setMetadata(obj->getImage().getMetadata());
      obj->setImage(img);
      obj->getImage().log(QString("OpCrop: %1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
      return OK;
    },
    "Automatically crop the image",py::arg("obj"),py::arg("threshold"),py::arg("neighbors"),py::arg("border"));
}
#endif

OpPlugin::ResultType OpCrop::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new OpCropDialog();
  QRect r = data.aoi;
  dlg->setSelection(r);
  if (dlg->exec())
  {
    if (dlg->isManual())
    {
      r = image->getImage().getOverlap(dlg->getSelection());
    }
    else
    {
      r = findArea(image->getImage(),dlg->getThreshold(),dlg->getNeighbors(),dlg->getBorder());
    }
    if (r.isValid())
    {
      profiler.start();
      auto img = image->getImage().subImage(r);
      img.setMetadata(image->getImage().getMetadata());
      image->setImage(img);
      profiler.stop();
      log(image,QString("OpCrop: %1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
      logProfiler(image);
      return OK;
    }
    else
    {
      QMessageBox::information(nullptr,"Crop...","No crop area found or specified!");
    }
  }
  return CANCELLED;
}


QRect OpCrop::findArea(const FitsImage& img, ValueType threshold, int neighbors, int border) const
{
  FitsImage gray = img.toGray();
  int xmin = std::numeric_limits<int>::max();
  int xmax = std::numeric_limits<int>::min();
  int ymin = std::numeric_limits<int>::max();
  int ymax = std::numeric_limits<int>::min();
  auto it = gray.getConstPixelIterator();
  for (int y=0;y<gray.getHeight();++y)
  {
    for (int x=0;x<gray.getWidth();++x)
    {
      if (it[0] >= threshold)
      {
        /* only mark as valid if neighbors count neighboring pixels are also
         * above the threshold. This only checks a line along the x and the y
         * axis and not the complete square.
         */
        bool flag = true;
        for (int j=1;j<neighbors&&x+j<gray.getWidth();++j)
        {
          auto it1 = it + j; //gray.getConstPixelIterator(x+j,y);
          if (it1[0] < threshold)
          {
            flag = false;
            break;
          }
        }
        if (flag)
        {
          for (int j=1;j<neighbors&&y+j<gray.getHeight();++j)
          {
            auto it1 = it + j * gray.getWidth(); //gray.getConstPixelIterator(x,y+j);
            if (it1[0] < threshold)
            {
              flag = false;
              break;
            }
          }
        }
        if (flag)
        {
          xmin = std::min(xmin,x);
          xmax = std::max(xmax,x);
          ymin = std::min(ymin,y);
          ymax = std::max(ymax,y);
        }
      }
      ++it;
    }
  }
  if (xmin >= xmax || ymin >= ymax) return QRect();
  xmin -= border;
  xmin = std::max(0,xmin);
  xmax += border;
  xmax = std::min(xmax,(int)gray.getWidth()-1);
  ymin -= border;
  ymin = std::max(0,ymin);
  ymax += border;
  ymax = std::min(ymax,(int)gray.getHeight()-1);
  return QRect(xmin,ymin,xmax-xmin+1,ymax-ymin+1);
}
