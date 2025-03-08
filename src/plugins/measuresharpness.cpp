/********************************************************************************
 *                                                                              *
 * FitsIP - measure the sharpness of images                                     *
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

#include "measuresharpness.h"
#include "measuresharpnessresultdialog.h"
#include "opkernel.h"
#include <fitsbase/kernelrepository.h>
#include <fitsbase/fitsimage.h>
#include <fitsbase/imagestatistics.h>
#include <fitsbase/dialogs/progressdialog.h>
#include <fitsbase/io/iofactory.h>
#include <fitsbase/math/average.h>
#include <algorithm>
#include <QApplication>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif


MeasureSharpness::MeasureSharpness()
{
  profiler = SimpleProfiler("MeasureSharpness");
  resultDialog = new MeasureSharpnessResultDialog();
  connect(resultDialog,&MeasureSharpnessResultDialog::writeToLogbook,this,&MeasureSharpness::copyToLog);
}

MeasureSharpness::~MeasureSharpness()
{
  resultDialog->deleteLater();
}

QString MeasureSharpness::getMenuEntry() const
{
  return "Measure/Sharpness...";
}

bool MeasureSharpness::requiresFileList() const
{
  return true;
}

#ifdef USE_PYTHON
void MeasureSharpness::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  py::class_<SharpnessData>(*m,"SharpnessData","Result of sharpness measurement")
      .def(py::init<>())
      .def_readwrite("min",&SharpnessData::min)
      .def_readwrite("max",&SharpnessData::max)
      .def_readwrite("mean",&SharpnessData::mean)
      .def_readwrite("variance",&SharpnessData::variance)
      .def_readwrite("min_pixel",&SharpnessData::minPixel)
      .def_readwrite("max_pixel",&SharpnessData::maxPixel)
      .def_readwrite("normalized_variance",&SharpnessData::normalizedVariance)
      ;
  m->def("calc_sharpness",[this](std::shared_ptr<FitsObject> obj){
        return calculateSharpness(obj->getImage());
      },
      "Calculate sharpness",py::arg("obj"));
  m->def("calc_sharpness",[this](std::shared_ptr<FitsObject> obj, int x, int y, int w, int h){
        return calculateSharpness(obj->getImage(),QRect(x,y,w,h));
      },
      "Calculate sharpness",py::arg("obj"),py::arg("x"),py::arg("y"),py::arg("w"),py::arg("h"));
}
#endif

OpPlugin::ResultType MeasureSharpness::execute(const std::vector<QFileInfo>& list, const OpPluginData& data)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
  Average normvaravg;
  int n = 0;
  for (const QFileInfo& info : list)
  {
    if (prog)
    {
      prog->setProgress(n++);
      prog->appendMessage(info.fileName());
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
    SharpnessData entry = evaluate(info,data.aoi);
    if (entry.info.exists())
    {
      results.push_back(entry);
      normvaravg.add(entry.normalizedVariance);
    }
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  log(QString::asprintf("Average sharpness: %g +- %g",normvaravg.getMean(),sqrt(normvaravg.getVariance())));
  std::sort(results.begin(),results.end(),[](const SharpnessData& e1, const SharpnessData& e2){return e1.normalizedVariance>e2.normalizedVariance;});
  resultDialog->setResult(results);
  resultDialog->exec();
  filelist = resultDialog->getFileList();
  return OK;
}

OpPlugin::ResultType MeasureSharpness::execute(const std::vector<std::shared_ptr<FitsObject>>& list, const OpPluginData& data)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
  Average normvaravg;
  int n = 0;
  for (const auto& obj : list)
  {
    QFileInfo info(obj->getFilename());
    if (prog)
    {
      prog->setProgress(n++);
      prog->appendMessage(info.fileName());
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
    SharpnessData result = calculateSharpness(obj->getImage(),data.aoi);
    result.info = info;
    result.filename = info.absoluteFilePath().toStdString();
    results.push_back(result);
    normvaravg.add(result.normalizedVariance);
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  log(QString::asprintf("Average sharpness: %g +- %g",normvaravg.getMean(),sqrt(normvaravg.getVariance())));
  std::sort(results.begin(),results.end(),[](const SharpnessData& e1, const SharpnessData& e2){return e1.normalizedVariance>e2.normalizedVariance;});
  QApplication::processEvents();
  resultDialog->setResult(results);
  resultDialog->exec();
  filelist = resultDialog->getFileList();
  return OK;
}



SharpnessData MeasureSharpness::evaluate(const QFileInfo info, QRect selection)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(info.absoluteFilePath());
  if (!handler) return SharpnessData();
  try
  {
    auto img = handler->read(info.absoluteFilePath());
    SharpnessData data = calculateSharpness(img,selection);
    data.info = info;
    data.filename = info.absoluteFilePath().toStdString();
    return data;
  }
  catch (std::exception& ex)
  {
  }
  return SharpnessData();
}

SharpnessData MeasureSharpness::calculateSharpness(std::shared_ptr<FitsImage> img, QRect selection) const
{
  SharpnessData data;
  auto copy = std::make_shared<FitsImage>(*img);
  if (selection.isValid())
  {
    copy = copy->subImage(selection);
  }
  ImageStatistics stat(*copy);
  data.minPixel = stat.getGlobalStatistics().minValue;
  data.maxPixel = stat.getGlobalStatistics().maxValue;
  /* apply laplacian */
  OpKernel op;
  const Kernel& kernel = KernelRepository::instance().getKernel(KernelRepository::LAPLACIAN);
  op.convolve(copy,kernel);
  /* ignore borders */
  QRect r(5,5,img->getWidth()-10,img->getHeight()-10);
  copy = copy->subImage(r);
  /* calculate statistics */
  Average avg;
  ConstPixelIterator p = copy->getConstPixelIterator();
  while (p.hasNext())
  {
    double v = std::fabs(p.getAbs());
    data.max = std::max(data.max,v);
    data.min = std::min(data.min,v);
    avg.add(v);
    ++p;
  }
  data.mean = avg.getMean();
  data.variance = avg.getVariance();
  double delta = data.maxPixel - data.minPixel;
  /*
   * The absolute value of the variance also depends on the image
   * brightness. The same image with half the brightness has only
   * a quarter of the variance. Thus we normalize with the square
   * of the maximum brightness difference to account for different
   * brightness.
   */
  if (delta >= 0) data.normalizedVariance = data.variance / (delta * delta);
  return data;
}


void MeasureSharpness::copyToLog()
{
  QString s = "";
  for (const SharpnessData& entry : results)
  {
    s += entry.info.fileName() + ": " + QString::number(entry.variance) + "\n";
  }
  log(std::shared_ptr<FitsImage>(),s);
}
