#include "s3sharpness.h"
#include "s3sharpnessresultdialog.h"
#include <fitsip/extensions/filter/opkernel.h>
#include <fitsip/core/kernelrepository.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/xydata.h>
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/dialogs/progressdialog.h>
#include <fitsip/core/io/iofactory.h>
#include <fitsip/core/math/average.h>
#include <fitsip/core/math/window/hanningwindow.h>
#include <algorithm>
#include <cmath>
#include <QApplication>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

static const double t1 = 5;
static const double t2 = 2;

S3Sharpness::S3Sharpness():
  spectralBlocksize(32),
  spectralOverlap(24),
  spatialBlocksize(8),
  sigmoid_t1(-3),
  sigmoid_t2(2),
  contrast_t1(5),
  contrast_t2(2),
  alpha(0.5)
{
  profiler = SimpleProfiler("S3Sharpness");
  resultDialog = new S3SharpnessResultDialog();
  connect(resultDialog,&S3SharpnessResultDialog::writeToLogbook,this,&S3Sharpness::copyToLog);
}

S3Sharpness::~S3Sharpness()
{
}

std::vector<std::shared_ptr<FitsObject>> S3Sharpness::getCreatedImages() const
{
  return images;
  // std::vector<std::shared_ptr<FitsObject>> list;
  // for (auto i : images)
  // {
  //   list.push_back(std::make_shared<FitsObject>(i));
  // }
  // return list;
}

QString S3Sharpness::getMenuEntry() const
{
  return "Analyse/S3 Sharpness...";
}

bool S3Sharpness::requiresFileList() const
{
  return true;
}

#ifdef USE_PYTHON
void S3Sharpness::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("calc_sharpness",[this](std::shared_ptr<FitsObject> obj){
    auto result = calculateSharpness(*obj->getImage(),contrast_t1,contrast_t2);
    if (result.images.size() < 3)
    {
      return std::make_tuple(result.s3,std::shared_ptr<FitsObject>(),std::shared_ptr<FitsObject>(),std::shared_ptr<FitsObject>());
    }
    else
    {
      return std::make_tuple(result.s3,result.images[0],result.images[1],result.images[2]);
    }
  },
  "Calculate sharpness",py::arg("obj"));
}
#endif

OpPlugin::ResultType S3Sharpness::execute(const std::vector<QFileInfo>& list, const OpPluginData& data)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
  images.clear();
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
    auto result = evaluate(info,data.aoi);
    if (result.info.exists())
    {
      images.insert(images.begin(),result.images.begin(),result.images.end());
      result.images.clear(); /* discard intermediate images */
      results.push_back(result);
      normvaravg.add(result.s3);
    }
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  log(QString::asprintf("Average sharpness: %g +- %g",normvaravg.getMean(),sqrt(normvaravg.getVariance())));
  std::sort(results.begin(),results.end(),[](const S3SharpnessData& e1, const S3SharpnessData& e2){return e1.s3>e2.s3;});
  resultDialog->setResult(results);
  resultDialog->exec();
  filelist = resultDialog->getFileList();
  return OK;
}

OpPlugin::ResultType S3Sharpness::execute(const std::vector<std::shared_ptr<FitsObject>>& list, const OpPluginData& data)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
  images.clear();
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
    FitsImage img(*obj->getImage());
    if (!data.aoi.isEmpty())
    {
      log("subimage");
      img = img.subImage(data.aoi);
    }
    auto result = calculateSharpness(img,contrast_t1,contrast_t2);
    result.info = info;
    result.filename = info.absoluteFilePath().toStdString();
    images.insert(images.begin(),result.images.begin(),result.images.end());
    result.images.clear(); /* discard intermediate images */
//    log(QString::asprintf("sharpness: %g",result.s3));
    results.push_back(result);
    normvaravg.add(result.s3);
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  log(QString::asprintf("Average sharpness: %g +- %g",normvaravg.getMean(),sqrt(normvaravg.getVariance())));
  std::sort(results.begin(),results.end(),[](const S3SharpnessData& e1, const S3SharpnessData& e2){return e1.s3>e2.s3;});
  QApplication::processEvents();
  resultDialog->setResult(results);
  resultDialog->exec();
  filelist = resultDialog->getFileList();
  return OK;
}

S3SharpnessData S3Sharpness::evaluate(const QFileInfo info, QRect selection)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(info.absoluteFilePath());
  if (!handler) return S3SharpnessData();
  try
  {
    FitsImage img(*handler->read(info.absoluteFilePath()).front()->getImage());
    if (!selection.isEmpty())
    {
      log("subimage");
      img = img.subImage(selection);
    }
    auto result = calculateSharpness(img,contrast_t1,contrast_t2);
    result.info = info;
    result.filename = info.absoluteFilePath().toStdString();
//    log(QString::asprintf("sharpness: %g",result.s3));
    return result;
  }
  catch (std::exception& ex)
  {
  }
  return S3SharpnessData();
}

S3SharpnessData S3Sharpness::calculateSharpness(const FitsImage& img, double t1, double t2) const
{
  S3SharpnessData sd;
  auto grayimg = img.toGray();
  grayimg.scaleIntensity(0,255);
  auto grayobj = std::make_shared<FitsObject>(std::make_shared<FitsImage>(grayimg));
//  images.push_back(grayobj);
  auto spectral = calculateSpectralSharpness(img.getLayer(0),t1,t2);
  {
    std::vector<Layer*> layers{spectral.first};
    auto specimg = std::make_shared<FitsImage>(img.getName()+"_spectral",layers);
    auto specobj = std::make_shared<FitsObject>(specimg);
    specobj->addXYData(spectral.second);
    sd.images.push_back(specobj);
  }
  auto spatial = calculateSpatialSharpness(img.getLayer(0));
  {
    std::vector<Layer*> layers{spatial};
    auto spatimg = std::make_shared<FitsImage>(img.getName()+"_spatial",layers);
    auto spatobj = std::make_shared<FitsObject>(spatimg);
    sd.images.push_back(spatobj);
  }
  auto s3 = new Layer(*spectral.first);
  ValueType* ptr = s3->getData();
  const ValueType* ptr2 = spatial->getData();
  for (size_t i=0;i<s3->size();++i)
  {
    *ptr = pow(*ptr,alpha) * pow(*ptr2,1-alpha);
    ++ptr;
    ++ptr2;
  }
  {
    std::vector<Layer*> layers{s3};
    auto s3img = std::make_shared<FitsImage>(img.getName()+"_sharpness",layers);
    auto s3obj = std::make_shared<FitsObject>(s3img);
    sd.images.push_back(s3obj);
  }
  std::vector<ValueType> s(s3->size());
  memcpy(s.data(),s3->getData(),s3->size());
  std::sort(s.begin(),s.end(),std::greater<ValueType>());
  double s3int = 0;
  for (size_t i=0;i<s.size()/100;++i)
  {
    s3int += s[i];
  }
  s3int /= s.size()/100;
  delete spectral.first;
  delete spatial;
  delete s3;
  sd.s3 = s3int;
  return sd;
}

std::pair<Layer*,std::vector<XYData>>  S3Sharpness::calculateSpectralSharpness(const Layer& layer, double t1, double t2) const
{
  int m = spectralBlocksize;
  int o = spectralOverlap;
  auto sl = new Layer(layer.getWidth(),layer.getHeight());
  std::vector<XYData> data;
  fftw_complex *s2c = new fftw_complex[m*(m/2+1)];
  double *in = new double[m*m];
  fftw_plan f = fftw_plan_dft_r2c_2d(m,m,in,s2c,FFTW_ESTIMATE);
  int y = 0;
  auto cl = calculateContrast(layer,m,t1,t2);
  HanningWindow w(m);
  while (y+m <= layer.getHeight())
  {
    int x = 0;
    while (x+m <= layer.getWidth())
    {
      ValueType contrast = 0;
      for (int j=0;j<m;++j)
      {
        ValueType* ptr = cl->getData() + (y + j) * cl->getWidth() + x;
        for (int i=0;i<m;++i)
        {
          contrast = std::max(contrast,*ptr);
          ++ptr;
        }
      }
      ValueType sharpness = 0;
      if (contrast > 0)
      {
        double* dptr = in;
        for (int j=0;j<m;++j)
        {
          const ValueType* ptr = layer.getData() + (y + j) * layer.getWidth() + x;
          for (int i=0;i<m;++i)
          {
            double v = *ptr++ * w[j] * w[i];
            *dptr++ = v;
          }
        }
        fftw_execute(f);
        auto fit = getSlope(s2c,m);
        double alpha = fabs(fit.first.getSlope());
        sharpness = 1 - 1 / (1 + exp(sigmoid_t1*(alpha-sigmoid_t2)));
        data.push_back(fit.second);
      }
      for (int j=0;j<m;++j)
      {
        ValueType* ptr = sl->getData() + (y + j) * sl->getWidth() + x;
        for (int i=0;i<m;++i)
        {
          *ptr++ = sharpness;
        }
      }
      x += m - o;
    }
    y += m - o;
  }
  fftw_destroy_plan(f);
  delete [] s2c;
  delete [] in;
  return std::make_pair(sl,data);
}

template <typename T> int sign(T val) {
  return (T(0) < val) - (val < T(0));
}

std::pair<LinearRegression,XYData> S3Sharpness::getSlope(fftw_complex* c, int m) const
{
  std::vector<double> s(m/2+1);
  std::vector<std::vector<double>> a(m);
  for (int j=0;j<m;++j)
  {
    a[j] = std::vector<double>(m/2+1);
    for (int i=0;i<m/2+1;++i)
    {
      double v = sqrt((*c)[0] * (*c)[0] + (*c)[1] * (*c)[1]);
      a[j][i] = v;
      ++c;
    }
  }
  std::vector<double> x,y;
  for (int r=1;r<=(m/2);++r) // for each polar frequency
  {
    double zs = 0;
    for (int ith=0;ith<360;++ith) // for each 1 degree angle
    {
        double th = 2 * M_PI * ith / 360.0; // convert to radian
        double x = r * sin(th); // x coordinate
        double y = r * cos(th); // y coordinate

        int x1 = sign(x) * floor ( abs (x) ); // rounding
        int x2 = sign(x) * ceil  ( abs (x) );
        if (x1 < 0 || x2 < 0) continue;
        int y1 = sign(y) * floor ( abs (y) );
        int y2 = sign(y) * ceil  ( abs (y) );

        double ex = abs(x - x1);
        double ey = abs(y - y1);

        if (x2<0)
        {
            ex = abs(x - x2);
            if (x1<0) x1 = m + x1;
            x2 = m + x2;
        }
        if (y2<0)
        {
            ey = abs(y - y2);
            if (y1<0) y1 = m + y1;
            y2 = m + y2;
        }

        double f11 = a[y1][x1]; //data(x1+1, y1+1);
        double f12 = a[y2][x1]; //data(x1+1, y2+1);
        double f21 = a[y1][x2]; //data(x2+1, y1+1);
        double f22 = a[y2][x2]; //data(x2+1, y2+1);

        double z = (f21-f11)*ex*(1-ey) + (f12-f11)*(1-ex)*ey + (f22-f11)*ex*ey + f11;

        zs = zs + z;
    }
    y.push_back(std::log(zs/360));
  }
  double delta = 0.5 / (m/2 + 1);
  for (int r=1;r<=(m/2);++r) // for each polar frequency
  {
    x.push_back(std::log(r*delta));
  }
  XYData data("Power Spectrum",x,y);
  LinearRegression r;
  try
  {
    r.execute(x,y);
  }
  catch (const std::exception& ex)
  {
//    log(ex.what());
    return std::make_pair(r,XYData());
  }
  //  std::cout << "slope=" << r.getSlope() << std::endl;
  return std::make_pair(r,data);
}

Layer* S3Sharpness::calculateSpatialSharpness(const Layer& layer) const
{
  int m = spatialBlocksize;
  auto sl = new Layer(layer.getWidth(),layer.getHeight());
  int y = 0;
  while (y+m <= layer.getHeight())
  {
    int x = 0;
    while (x+m < layer.getWidth())
    {
      ValueType tv_max = 0;
      for (int j=0;j<m-1;++j)
      {
        const ValueType* ptr1 = layer.getData() + (y + j) * layer.getWidth() + x;
        const ValueType* ptr2 = layer.getData() + (y + j + 1) * layer.getWidth() + x;
        for (int i=0;i<m-1;++i)
        {
          ValueType d = (abs(ptr1[0]-ptr2[0])
                     + abs(ptr1[0]-ptr1[1])
                     + abs(ptr1[0]-ptr2[1])
                     + abs(ptr2[1]-ptr1[1])
                     + abs(ptr1[1]-ptr2[0])
                     + abs(ptr2[1]-ptr2[0])) / 255;
          tv_max = std::max(tv_max,d);
          ++ptr1;
          ++ptr2;
        }
      }
      tv_max /= 4;
      for (int j=0;j<m;++j)
      {
        ValueType* ptr = sl->getData() + (y + j) * sl->getWidth() + x;
        for (int i=0;i<m;++i)
        {
          *ptr++ = tv_max;
        }
      }
      x += m;
    }
    y += m;
  }
  return sl;
}

std::unique_ptr<Layer> S3Sharpness::calculateContrast(const Layer& layer, int m, double t1, double t2) const
{
  auto cl = std::make_unique<Layer>(layer);
  /* calculate  luminosity */
  ValueType* ptr = cl->getData();
  for (size_t i=0;i<cl->size();++i)
  {
    *ptr = pow(0.7657+0.0364*(*ptr),2.2);
    ++ptr;
  }
  int d = m / 2;
  for (int y=0;y<=cl->getHeight()-m;y+=d)
  {
    for (int x=0;x<=cl->getWidth()-m;x+=d)
    {
      double sx = 0;
      double sx2 = 0;
      double lmin = std::numeric_limits<double>::max();
      double lmax = std::numeric_limits<double>::min();
      for (int j=0;j<m;++j)
      {
        ValueType* ptr = cl->getData() + (y + j) * cl->getWidth() + x;
        for (int i=0;i<m;++i)
        {
          sx += *ptr;
          sx2 += *ptr * *ptr;
          lmin = std::min(lmin,(double)*ptr);
          lmax = std::max(lmax,(double)*ptr);
          ++ptr;
        }
      }
      double mean = sx / (m * m);
      double stddev = sqrt(sx2/m - mean*mean);
      if (mean > 127.5) mean = 255.0 - mean;
      ValueType contrast = 0;
      if (mean > t2 && (lmax-lmin) > t1)
      {
        contrast = stddev / mean;
      }
      if (contrast > t1) contrast = t1;
      contrast /= t1;
      for (int j=0;j<m;++j)
      {
        ValueType* ptr = cl->getData() + (y + j) * cl->getWidth() + x;
        for (int i=0;i<m;++i)
        {
          *ptr = contrast;
          ++ptr;
        }
      }
    }
  }
  return cl;
}

void S3Sharpness::copyToLog()
{
  QString s = "";
  for (const S3SharpnessData& entry : results)
  {
    s += entry.info.fileName() + ": " + QString::number(entry.s3) + "\n";
  }
  log(nullptr,s);
}
