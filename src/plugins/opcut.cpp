/********************************************************************************
 *                                                                              *
 * FitsIP - cut low and high values                                             *
 *                                                                              *
 * modified: 2025-02-01                                                         *
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

#include "opcut.h"
#include <fitsbase/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpCut::OpCut()
{
  Q_INIT_RESOURCE(pluginresources);
  dlg.setTitle("Cut Image");
  dlg.setValue1Label("Lower limit:");
  dlg.setValue2Label("Upper limit:");
  dlg.setValue1("0.0");
  dlg.setValue2(QString::number(std::numeric_limits<ValueType>::max()));
  profiler = SimpleProfiler("OpCut");
}

OpCut::~OpCut()
{
}

QString OpCut::getMenuEntry() const
{
  return "Math/Cut...";
}

QIcon OpCut::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/opcut.png");
}

#ifdef USE_PYTHON
void OpCut::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("cut",[this](std::shared_ptr<FitsObject> obj, ValueType lo, ValueType hi){
    cut(obj->getImage(),lo,hi);
    return OK;
  },
  "Cut values outside a given range",py::arg("obj"),py::arg("lo"),py::arg("hi"));
}
#endif

OpPlugin::ResultType OpCut::execute(std::shared_ptr<FitsObject> image, QRect /*selection*/, const PreviewOptions& opt)
{
  if (dlg.exec())
  {
    ValueType lower = dlg.getValue1().toDouble();
    ValueType upper = dlg.getValue2().toDouble();
    profiler.start();
    cut(image->getImage(),lower,upper);
    profiler.stop();
    log(image,QString("cut values ouside range: lower=%1 upper=%2").arg(lower).arg(upper));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

void OpCut::cut(std::shared_ptr<FitsImage> image, ValueType lower, ValueType upper) const
{
  PixelIterator p = image->getPixelIterator();
  while (p.hasNext())
  {
    if (p.min() < lower)
      p.set(lower);
    else if (p.max() > upper)
      p.set(upper);
    ++p;
  }
}
