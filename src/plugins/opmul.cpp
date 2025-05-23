/********************************************************************************
 *                                                                              *
 * FitsIP - multiply two images                                                 *
 *                                                                              *
 * modified: 2025-01-31                                                         *
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

#include "opmul.h"
#include <fitsbase/imagecollection.h>
#include <fitsbase/fitsobject.h>
#include <fitsbase/fitsimage.h>
#include <QDebug>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpMul::OpMul():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpMul");
}

OpMul::~OpMul()
{
  if (dlg) dlg->deleteLater();
}

QString OpMul::getMenuEntry() const
{
  return "Math/Mul...";
}

QIcon OpMul::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/mul.png");
}

#ifdef USE_PYTHON
void OpMul::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("mul",[](std::shared_ptr<FitsObject> obj1, std::shared_ptr<FitsObject> obj2){
    *(obj1->getImage()) *= *(obj2->getImage());
    obj1->getImage()->log("Multiplied by image "+obj2->getImage()->getName());
    return OK;
  },
  "Multiply the first image with the second one",py::arg("obj1"),py::arg("obj2"));
}
#endif

OpPlugin::ResultType OpMul::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (dlg == nullptr)
  {
    dlg = new ImageSelectDialog();
    dlg->setTitle("Multiply by Image");
    dlg->setImageCollection(data.imageCollection);
  }
  dlg->setPrompt("Multiply "+image->getName()+" by:");
  if (dlg->exec())
  {
    std::shared_ptr<FitsObject> file = dlg->getImage();
    profiler.start();
    try
    {
      *(image->getImage()) *= *file->getImage();
      profiler.stop();
      log(image,"Multiplied by image "+file->getImage()->getName());
      logProfiler(image);
    }
    catch (const std::exception& ex)
    {
      setError(ex.what());
      qWarning() << ex.what();
      return ERROR;
    }
    return OK;
  }
  return CANCELLED;
}

