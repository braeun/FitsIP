/********************************************************************************
 *                                                                              *
 * FitsIP - crop image                                                          *
 *                                                                              *
 * modified: 2023-02-04                                                         *
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
#include <fitsbase/fitsimage.h>

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
  m->def("cut",[](std::shared_ptr<FitsObject> obj, int x, int y, int w, int h){
    QRect r(x,y,w,h);
    auto img = obj->getImage()->subImage(r);
    img->setMetadata(obj->getImage()->getMetadata());
    obj->setImage(img);
    obj->getImage()->log(QString("OpCrop: %1,%2 %3x%4").arg(x).arg(y).arg(w).arg(h));
    return OK;
  },
  "Crop the image",py::arg("obj"),py::arg("x"),py::arg("y"),py::arg("w"),py::arg("h"));
}
#endif

OpPlugin::ResultType OpCrop::execute(std::shared_ptr<FitsObject> image, QRect r, const PreviewOptions& opt)
{
  if (!dlg) dlg = new OpCropDialog();
  dlg->setSelection(r);
  if (dlg->exec())
  {
    r = image->getImage()->getOverlap(dlg->getSelection());
    if (r.isValid())
    {
      profiler.start();
      auto img = image->getImage()->subImage(r);
      img->setMetadata(image->getImage()->getMetadata());
      image->setImage(img);
      profiler.stop();
      log(image,QString("OpCrop: %1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
      logProfiler(image);
      return OK;
    }
  }
  return CANCELLED;
}
