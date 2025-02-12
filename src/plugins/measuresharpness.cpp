#include "measuresharpness.h"
#include "measuresharpnessresultdialog.h"
#include "opkernel.h"
#include <fitsbase/kernelrepository.h>
#include <fitsbase/fitsimage.h>
#include <fitsbase/imagestatistics.h>
#include <fitsbase/dialogs/progressdialog.h>
#include <fitsbase/io/iofactory.h>
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
}

MeasureSharpness::~MeasureSharpness()
{
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

OpPlugin::ResultType MeasureSharpness::execute(const std::vector<QFileInfo>& list, QRect selection, const PreviewOptions& opt)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
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
    SharpnessData entry = evaluate(info,selection);
    if (entry.info.exists()) results.push_back(entry);
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  std::sort(results.begin(),results.end(),[](const SharpnessData& e1, const SharpnessData& e2){return e1.normalizedVariance>e2.normalizedVariance;});
  MeasureSharpnessResultDialog d;
  d.setResult(results);
  connect(&d,&MeasureSharpnessResultDialog::writeToLogbook,this,&MeasureSharpness::copyToLog);
  d.exec();
  disconnect(&d);
  filelist = d.getFileList();
  return OK;
}

OpPlugin::ResultType MeasureSharpness::execute(const std::vector<std::shared_ptr<FitsObject>>& list, QRect selection, const PreviewOptions& opt)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
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
    SharpnessData data = calculateSharpness(obj->getImage(),selection);
    data.info = info;
    data.filename = info.absoluteFilePath().toStdString();
    results.push_back(data);
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  std::sort(results.begin(),results.end(),[](const SharpnessData& e1, const SharpnessData& e2){return e1.normalizedVariance>e2.normalizedVariance;});
  QApplication::processEvents();
  MeasureSharpnessResultDialog d;
  d.setResult(results);
  connect(&d,&MeasureSharpnessResultDialog::writeToLogbook,this,&MeasureSharpness::copyToLog);
  d.exec();
  disconnect(&d);
  filelist = d.getFileList();
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
  int n = 0;
  double gsum = 0;
  double gsum2 = 0;
  ConstPixelIterator p = copy->getConstPixelIterator();
  while (p.hasNext())
  {
    double v = std::fabs(p.getAbs());
    data.max = std::max(data.max,v);
    data.min = std::min(data.min,v);
    gsum += v;
    gsum2 += v * v;
    n++;
    ++p;
  }
  if (n > 0)
  {
    data.mean = gsum / n;
    if (n > 1)
    {
      data.variance = (n*gsum2-gsum*gsum)/(n*static_cast<double>(n-1));
      double delta = data.maxPixel - data.minPixel;
      /*
       * The absolute value of the variance also depends on the image
       * brightness. The same image with half the brightness has only
       * a quarter of the variance. Thus we normalize with the square
       * of the maximum brightness difference to account for different
       * brightness.
       */
      if (delta >= 0) data.normalizedVariance = data.variance / (delta * delta);
    }
  }
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
