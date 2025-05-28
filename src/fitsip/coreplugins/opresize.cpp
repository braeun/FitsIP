/********************************************************************************
 *                                                                              *
 * FitsIP - resize image                                                        *
 *                                                                              *
 * modified: 2025-02-26                                                         *
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

#include "opresize.h"
#include "opresizedialog.h"
#include <fitsip/core/fitsimage.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpResize::OpResize():
  dlg(nullptr)
{
  Q_INIT_RESOURCE(pluginresources);
  profiler = SimpleProfiler("OpResize");
}

OpResize::~OpResize()
{
if (dlg) dlg->deleteLater();
}

QString OpResize::getMenuEntry() const
{
return "Image/Resize...";
}

QIcon OpResize::getIcon() const
{
  return QIcon(":/pluginicons/resources/icons/transform-scale.png");
}

#ifdef USE_PYTHON
void OpResize::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("resize",[this](std::shared_ptr<FitsObject> obj, double factor, char mode){
    scriptResize(obj,factor,factor,mode);
    return OK;
  },
  "Resize the image by a given factor",py::arg("obj"),py::arg("factor"),py::arg("mode")='b');
  m->def("resize",[this](std::shared_ptr<FitsObject> obj, double factorx, double factory, char mode){
    scriptResize(obj,factorx,factory,mode);
    return OK;
  },
  "Resize the image by a given factor",py::arg("obj"),py::arg("factorx"),py::arg("factory"),py::arg("mode")='b');

  //  m->def("shrink",[this](std::shared_ptr<FitsObject> obj, double factor){
//    auto img = shrink(obj->getImage(),factor,factor);
//    obj->setImage(img);
//    obj->getImage()->log(QString("Resize shrink: factor=%1").arg(factor));
//    return OK;
//  },
//  "Shrink the image by a given factor",py::arg("obj"),py::arg("factor"));
//  m->def("shrink",[this](std::shared_ptr<FitsObject> obj, double factorx, double factory){
//    auto img = shrink(obj->getImage(),factorx,factory);
//    obj->setImage(img);
//    obj->getImage()->log(QString("Resize shrink: factor x=%1   factor y=%1").arg(factorx).arg(factory));
//    return OK;
//  },
//  "Shrink the image by a given factor",py::arg("obj"),py::arg("factorx"),py::arg("factory"));
//  m->def("grow",[this](std::shared_ptr<FitsObject> obj, double factor){
//    auto img = growBilinear(obj->getImage(),factor,factor);
//    obj->setImage(img);
//    obj->getImage()->log(QString("Resize grow: factor=%1 %2").arg(factor).arg("bilinear"));
//    return OK;
//  },
//  "Grow the image by a given factor with optional bilinear interpolation",py::arg("obj"),py::arg("factor"));
//  m->def("grow",[this](std::shared_ptr<FitsObject> obj, double factorx, double factory){
//    auto img = growBilinear(obj->getImage(),factorx,factory);
//    obj->setImage(img);
//    obj->getImage()->log(QString("Resize grow: factor x=%1   factor y=%2  (%3)").arg(factorx).arg(factory).arg("bilinear"));
//    return OK;
//  },
//  "Grow the image by a given factor with optional bilinear interpolation",py::arg("obj"),py::arg("factorx"),py::arg("factory"));
}
#endif

OpPlugin::ResultType OpResize::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (dlg == nullptr)
  {
    dlg = new OpResizeDialog();
  }
  dlg->setCurrentSize(image->getImage()->getWidth(),image->getImage()->getHeight());
  if (dlg->exec())
  {
    double factorx = dlg->getFactorX();
    double factory = dlg->getFactorY();
    QString mode = "no scaling";
    int m = 0;
    if (dlg->isNearestNeighborInterpolation())
    {
      m = 1;
      mode = "nearest neighbor";
    }
    else if (dlg->isBilinearInterpolation())
    {
      m = 2;
      mode = "bilinear";
    }
    profiler.start();
    image->setImage(resize(image->getImage(),factorx,factory,m));
    profiler.stop();
    log(image,QString("Resize: factor x=%1 y=%2 %3").arg(factorx).arg(factory).arg(mode));
    logProfiler(image);
    return OK;
  }
  return CANCELLED;
}

std::shared_ptr<FitsImage> OpResize::resize(std::shared_ptr<FitsImage> image, double factorx, double factory, int mode) const
{
  if (mode == 0)
  {
    int w = image->getWidth() * factorx;
    int h = image->getHeight() * factory;
    return image->resizedImage(w,h);
  }
  if (factorx < 1 && factory < 1)
  {
    return shrink(image,factorx,factory);
  }
  else if (factorx < 1)
  {
    image = shrink(image,factorx,1.0);
    factorx = 1.0;
  }
  else if (factory < 1)
  {
    image = shrink(image,1.0,factory);
    factory = 1.0;
  }
  if (mode == 1)
  {
    return growNearestNeighbor(image,factorx,factory);
  }
  else //if (mode == 2)
  {
    return growBilinear(image,factorx,factory);
  }
}



//std::shared_ptr<FitsImage> OpResize::grow(std::shared_ptr<FitsImage> image, int factor, bool bilinear) const
//{
//  int w = image->getWidth() * factor;
//  int h = image->getHeight() * factor;
//  auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getDepth());
//  img->setMetadata(image->getMetadata());
//  PixelIterator it = img->getPixelIterator();
//  if (bilinear)
//  {
//    for (int y=0;y<h;y++)
//    {
//      for (int x=0;x<w;x++)
//      {
//        int xp = x / factor;
//        int yp = y / factor;
//        for (int d=0;d<image->getDepth();d++)
//        {
//          double A = image->getConstPixelIterator(xp,yp)[d];
//          double B = A;
//          if (xp+1 < image->getWidth()) B = image->getConstPixelIterator(xp+1,yp)[d];
//          double C = A;
//          if (yp+1 < image->getHeight()) C = image->getConstPixelIterator(xp,yp+1)[d];
//          double D = A;
//          if (yp+1 < image->getHeight() && xp+1 < image->getWidth()) D = image->getConstPixelIterator(xp+1,yp+1)[d];
//          double a = static_cast<double>(x) / static_cast<double>(factor) - xp;
//          double b = static_cast<double>(y) / static_cast<double>(factor) - yp;
//          double v = (a - 1) * (b - 1) * A + a * (1 - b) * B + (1 - a) * b * C + a * b * D;
//          it[d] = v;
//          ++it;
//        }
//      }
//    }
//  }
//  else
//  {
//    for (int y=0;y<h;y++)
//    {
//      for (int x=0;x<w;x++)
//      {
//        int xp = x / factor;
//        int yp = y / factor;
//        ConstPixelIterator it2 = image->getConstPixelIterator(xp,yp);
//        for (int d=0;d<image->getDepth();d++)
//        {
//          it[d] = it2[d];
//        }
//        ++it;
//      }
//    }
//  }
//  return img;
//}

//std::shared_ptr<FitsImage> OpResize::shrink(std::shared_ptr<FitsImage> image, int factor) const
//{
//  int w = image->getWidth() / factor;
//  int h = image->getHeight() / factor;
//  auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getDepth());
//  img->setMetadata(image->getMetadata());
//  for (int d=0;d<image->getDepth();d++)
//  {
//    ValueType* src = image->getLayer(d)->getData();
//    for (int y=0;y<h*factor;y++)
//    {
//      ValueType* dst = img->getLayer(d)->getData() + (y / factor * w);
//      for (int x=0;x<w*factor;x++)
//      {
//        dst[x/factor] += src[x];
//      }
//      src += image->getWidth();
//    }
//  }
//  return img;
//}

std::shared_ptr<FitsImage> OpResize::shrink(std::shared_ptr<FitsImage> image, double factorx, double factory) const
{
  double sx = 1 / factorx;
  double sy = 1 / factory;
  int w = static_cast<int>(image->getWidth() * factorx) + 1;
  int h = static_cast<int>(image->getHeight() * factory) + 1;
  auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getDepth());
  img->setMetadata(image->getMetadata());
  for (int d=0;d<image->getDepth();d++)
  {
    ValueType* src = image->getLayer(d)->getData();
    ValueType* dst = img->getLayer(d)->getData();
    double ys = sy;
    for (int y=0;y<image->getHeight();y++)
    {
      double fy = (ys >= 1.0) ? 1.0 : ys;
      double xs = sx;
      int xd = 0;
      for (int x=0;x<image->getWidth()&&xd<w;x++)
      {
        if (xs >= 1.0)
        {
          dst[xd] += fy * src[x];
          xs -= 1.0;
        }
        else
        {
          dst[xd] += fy * xs * src[x];
          if (xd+1 < w) dst[xd+1] += fy * (1 - xs) * src[x];
          xs += sx - 1;
          ++xd;
        }
      }
      if (ys < 1.0)
      {
        dst += img->getWidth();
        fy = 1 - ys;
        double xs = sy;
        int xd = 0;
        for (int x=0;x<image->getWidth()&&xd<w;x++)
        {
          if (xs >= 1.0)
          {
            dst[xd] += fy * src[x];
            xs -= 1.0;
          }
          else
          {
            dst[xd] += fy * xs * src[x];
            if (xd+1 < w) dst[xd+1] += fy * (1 - xs) * src[x];
            xs +=sx - 1;
            ++xd;
          }
        }
        ys += sy - 1;
      }
      else
      {
        ys -= 1.0;
      }
      src += image->getWidth();
    }
  }
  return img;
}

std::shared_ptr<FitsImage> OpResize::growNearestNeighbor(std::shared_ptr<FitsImage> image, double factorx, double factory) const
{
  int w = static_cast<int>(image->getWidth() * factorx);
  int h = static_cast<int>(image->getHeight() * factory);
  auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getDepth());
  img->setMetadata(image->getMetadata());
  for (int d=0;d<image->getDepth();++d)
  {
    ValueType* dst = img->getLayer(d)->getData();
    for (int y=0;y<img->getHeight();++y)
    {
      int ys1 = static_cast<int>(round(y/factory));
      if (ys1 >= image->getHeight()) ys1 = image->getHeight() - 1;
      ValueType* src = image->getLayer(d)->getData() + ys1 * image->getWidth();
      for (int x=0;x<img->getWidth();++x)
      {
        int xs1 = static_cast<int>(round(x/factorx));
        if (xs1 >= image->getWidth()) xs1 = image->getWidth() - 1;
        dst[x] = src[xs1];
      }
      dst += img->getWidth();
    }
  }
  return img;
}

std::shared_ptr<FitsImage> OpResize::growBilinear(std::shared_ptr<FitsImage> image, double factorx, double factory) const
{
  int w = static_cast<int>(image->getWidth() * factorx);
  int h = static_cast<int>(image->getHeight() * factory);
  auto img = std::make_shared<FitsImage>(image->getName(),w,h,image->getDepth());
  img->setMetadata(image->getMetadata());
  for (int d=0;d<image->getDepth();++d)
  {
    ValueType* dst = img->getLayer(d)->getData();
    for (int y=0;y<img->getHeight();++y)
    {
      double ys = y / factory;
      int ys1 = static_cast<int>(ys);
      ValueType* src = image->getLayer(d)->getData() + ys1 * image->getWidth();
      if (ys1+1 >= image->getHeight())
        break;
      ys -= ys1;
      for (int x=0;x<img->getWidth();++x)
      {
        double xs = x / factorx;
        int xs1 = static_cast<int>(xs);
        if (xs1+1 >= image->getWidth())
          break;
        xs -= xs1;
        dst[x] = (1 - xs) * (1 - ys) * src[xs1]
            + xs * (1 - ys) * src[xs1+1]
            + (1 - xs) * ys * src[image->getWidth()+xs1]
            + xs * ys * src[image->getWidth()+xs1+1];
      }
      dst += img->getWidth();
    }
  }
  return img;
}


void OpResize::scriptResize(std::shared_ptr<FitsObject> obj, double factorx, double factory, char mode) const
{
  int m = 0;
  QString md = "no scaling";
  if (mode == 'n')
  {
    m = 1;
    md = "nearest neighbor";
  }
  else if (mode == 'b')
  {
    m = 2;
    md = "bilinear";
  }
  auto img = resize(obj->getImage(),factorx,factory,m);
  obj->setImage(img);
  obj->getImage()->log(QString("Resize: factor x=%1  factor y=%2 %3").arg(factorx).arg(factory).arg(md));
}

