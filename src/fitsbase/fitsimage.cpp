/********************************************************************************
 *                                                                              *
 * FitsIP - image object                                                        *
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

#include "fitsimage.h"
#include <algorithm>


Layer::Layer(int w, int h):
  width(w),
  height(h)
{
  data = new ValueType[w*h];
  memset(data,0,w*h*sizeof(ValueType));
}

Layer::Layer(const Layer& l):
  width(l.width),
  height(l.height)
{
  data = new ValueType[width*height];
  memcpy(data,l.data,width*height*sizeof(ValueType));
}

Layer::~Layer()
{
  if (data) delete [] data;
}

void Layer::setData(std::valarray<ValueType> &d)
{
  ValueType *p = data;
  for (size_t i=0;i<std::min(static_cast<size_t>(width*height),static_cast<size_t>(d.size()));i++)
  {
    *p++ = d[i];
  }
}



FitsImage::FitsImage():
  preFFTWidth(0),
  preFFTHeight(0),
  name(""),
  width(0),
  height(0),
  depth(0)
{
}

FitsImage::FitsImage(const QString& name, int w, int h, int d):
  preFFTWidth(0),
  preFFTHeight(0),
  name(name),
  width(w),
  height(h),
  depth(d)
{
  for (int i=0;i<d;i++)
  {
    layers.push_back(std::make_shared<Layer>(w,h));
  }
}

FitsImage::FitsImage(const FitsImage& img):
  preFFTWidth(img.preFFTWidth),
  preFFTHeight(img.preFFTHeight),
  name(img.name),
  width(img.width),
  height(img.height),
  depth(img.depth),
  metadata(img.metadata)
{
  for (int i=0;i<depth;i++)
  {
    layers.push_back(std::make_shared<Layer>(*img.layers[i]));
  }
}

FitsImage::FitsImage(FitsImage&& img):
  preFFTWidth(img.preFFTWidth),
  preFFTHeight(img.preFFTHeight),
  name(img.name),
  width(img.width),
  height(img.height),
  depth(img.depth),
  layers(img.layers),
  metadata(img.metadata)
{
}

FitsImage::FitsImage(const QString& name, const FitsImage& img):
  preFFTWidth(img.preFFTWidth),
  preFFTHeight(img.preFFTHeight),
  name(name),
  width(img.width),
  height(img.height),
  depth(img.depth),
  metadata(img.metadata)
{
  for (int i=0;i<depth;i++)
  {
    layers.push_back(std::make_shared<Layer>(*img.layers[i]));
  }
}

bool FitsImage::isNull() const
{
  return width == 0 || height == 0;
}

QString FitsImage::getName() const
{
  return name;
}

Pixel FitsImage::getPixel(int x, int y) const
{
  if (x < 0) x += width;
  if (y < 0) y += height;
  Pixel px;
  px.x = x;
  px.y = y;
  ConstPixelIterator it = getConstPixelIterator(x,y);
  for (int d=0;d<getDepth();d++) px.i.push_back(it[d]);
  px.v = it.getAbs();
  return px;
}

Pixel FitsImage::getPixel(const ConstPixelIterator &it) const
{
  Pixel px;
  px.x = it.getPos() % width;
  px.y = it.getPos() / width;
  for (int d=0;d<getDepth();d++) px.i.push_back(it[d]);
  px.v = it.getAbs();
  return px;
}

PixelIterator FitsImage::getPixelIterator()
{
  int size = width * height;
  std::vector<ValueType*> l;
  for (const std::shared_ptr<Layer>& layer : layers) l.push_back(layer->getData());
  return PixelIterator(size,l);
}

PixelIterator FitsImage::getPixelIterator(int x, int y)
{
  if (x < 0) x += width;
  if (y < 0) y += height;
  PixelIterator it = getPixelIterator();
  it += y * width + x;
  return it;
}

ConstPixelIterator FitsImage::getConstPixelIterator() const
{
  int size = width * height;
  std::vector<const ValueType*> l;
  for (const std::shared_ptr<Layer>& layer : layers) l.push_back(layer->getData());
  return ConstPixelIterator(size,l);
}

ConstPixelIterator FitsImage::getConstPixelIterator(int x, int y) const
{
  if (x < 0) x += width;
  if (y < 0) y += height;
  ConstPixelIterator it = getConstPixelIterator();
  it += y * width + x;
  return it;
}

const ImageMetadata& FitsImage::getMetadata() const
{
  return metadata;
}

void FitsImage::setMetadata(const ImageMetadata& data)
{
  metadata = data;
}

void FitsImage::log(QString msg)
{
  metadata.history.push_back(msg);
}

bool FitsImage::isCompatible(const FitsImage &img) const
{
  if (depth != img.depth) return false;
  if (width != img.width) return false;
  if (height != img.height) return false;
  return true;
}

QRect FitsImage::getOverlap(const QRect& r) const
{
//  int32_t x = r.x();
//  int32_t y = r.y();
//  int32_t w = r.width();
//  int32_t h = r.height();
//  if (x >= width) return QRect();
//  if (y >= height) return QRect();
//  if (x < 0) x = 0;
//  if (y < 0) y = 0;
//  if (x+w >= width) w = static_cast<int32_t>(width) - x;
//  if (y+h >= height) h = static_cast<int32_t>(height) - y;
//  QRect r1 = QRect(x,y,w,h);
  QRect r1 = QRect(0,0,width,height) & r;
  return r1;
}

std::shared_ptr<FitsImage> FitsImage::subImage(const QRect &r) const
{
  QRect a = getOverlap(r);
  if (a.isNull()) return std::shared_ptr<FitsImage>();
  int x = a.x();
  int y = a.y();
  int w = a.width();
  int h = a.height();
  auto img = std::make_shared<FitsImage>(name,w,h,getDepth());
  for (int d=0;d<getDepth();d++)
  {
    ValueType* dst = img->getLayer(d)->getData();
    ValueType* src = getLayer(d)->getData() + y * getWidth() + x;
    for (int y=0;y<h;y++)
    {
      memcpy(dst,src,w*sizeof(ValueType));
      dst += w;
      src += getWidth();
    }
  }
  return img;
}

std::shared_ptr<FitsImage> FitsImage::resizedImage(int w, int h) const
{
  auto img = std::make_shared<FitsImage>(name,w,h,getDepth());
  int xoff = (w - width) / 2;
  int yoff = (h - height) / 2;
  for (int d=0;d<getDepth();d++)
  {
    int xs = xoff >= 0 ? 0 : -xoff;
    int xd = xoff >= 0 ? xoff : 0;
    int xcnt = xoff >= 0 ? width : w;
    int ys = yoff >= 0 ? 0 : -yoff;
    int yd = yoff >= 0 ? yoff : 0;
    int lines = yoff >= 0 ? height : h;
    ValueType* dst = img->getLayer(d)->getData() + yd * w + xd;
    ValueType* src = getLayer(d)->getData() + ys * width + xs;
    for (int i=0;i<lines;++i)
    {
      memcpy(dst,src,xcnt*sizeof(ValueType));
      dst += w;
      src += width;
    }
  }
  return img;
}


std::shared_ptr<FitsImage> FitsImage::toGray()
{
  std::shared_ptr<FitsImage>  gray = std::make_shared<FitsImage>(getName(),getWidth(),getHeight(),1);
  gray->setMetadata(getMetadata());
  ConstPixelIterator src = getConstPixelIterator();
  PixelIterator dest = gray->getPixelIterator();
  while (src.hasNext())
  {
    dest[0] = src.getRGB().gray();
    ++src;
    ++dest;
  }
  return gray;
}



Pixel FitsImage::getBrightestPixel(const QRect &r) const
{
  Pixel pixel;
  for (int y=0;y<r.height();y++)
  {
    ConstPixelIterator it = getConstPixelIterator(r.x(),r.y()+y);
    for (int x=0;x<r.width();x++)
    {
      double val = it.getAbs();
      if (val > pixel.v)
      {
        pixel.v = val;
        pixel.x = x + r.x();
        pixel.y = y + r.y();
        for (int d=0;d<getDepth();d++) pixel.i.push_back(it[d]);
      }
      ++it;
    }
  }
  return pixel;
}

FitsImage& FitsImage::operator+=(const FitsImage& img)
{
  if (!isCompatible(img)) throw std::runtime_error("Incompatible fits image");
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    ValueType *p1 = img.getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p += *p1;
      ++p;
      ++p1;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator+=(ValueType v)
{
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p += v;
      ++p;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator-=(const FitsImage& img)
{
  if (!isCompatible(img)) throw std::runtime_error("Incompatible fits image");
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    ValueType *p1 = img.getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p -= *p1;
      ++p;
      ++p1;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator-=(ValueType v)
{
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p -= v;
      ++p;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator*=(const FitsImage& img)
{
  if (!isCompatible(img)) throw std::runtime_error("Incompatible fits image");
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    ValueType *p1 = img.getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p *= *p1;
      ++p;
      ++p1;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator*=(ValueType v)
{
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p *= v;
      ++p;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator/=(const FitsImage& img)
{
  if (!isCompatible(img)) throw std::runtime_error("Incompatible fits image");
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    ValueType *p1 = img.getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      if (fabs(*p1) > 1.0E-20) *p /= *p1;
      ++p;
      ++p1;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator/=(ValueType v)
{
  for (int d=0;d<getDepth();d++)
  {
    ValueType *p = getLayer(d)->getData();
    int n = width * height;
    while (n-- > 0)
    {
      *p /= v;
      ++p;
    }
  }
  return *this;
}

FitsImage& FitsImage::operator=(const FitsImage& img)
{
  name = img.name;
  width = img.width;
  height = img.height;
  depth = img.depth;
  metadata = img.metadata;
  layers.clear();
  for (int i=0;i<depth;i++)
  {
    layers.push_back(std::make_shared<Layer>(*img.layers[i]));
  }
  preFFTHeight = img.preFFTHeight;
  preFFTWidth = img.preFFTWidth;
  return *this;
}


QImage FitsImage::toQImage(ValueType min, ValueType max, Scale scale) const
{
  switch (scale)
  {
    case LINEAR:
      return toQImageLin(min,max);
    case LOG:
      return toQImageLog(min,max);
    case SQRT:
      return toQImageSqrt(min,max);
    case SINE:
      return toQImageSine(min,max);
  }
  return toQImageLin(min,max);
}

QImage FitsImage::toQImageLin(ValueType min, ValueType max) const
{
  uint32_t* data = new uint32_t[width*height];
  ValueType scale = 256 / (max - min);
  uint32_t* d = data;
  ConstPixelIterator p = getConstPixelIterator();
  *d++ = p.getRGB().toUInt(min,scale);
  while (p.hasNext())
  {
    ++p;
    *d++ = p.getRGB().toUInt(min,scale);
  }
  QImage img(reinterpret_cast<const uchar*>(data),static_cast<int32_t>(width),static_cast<int32_t>(height),QImage::Format_RGB32);
  return img;
}

QImage FitsImage::toQImageLog(ValueType min, ValueType max) const
{
  uint32_t* data = new uint32_t[width*height];
  if (min <= 0) min = 0.1f;
  if (max <= 0) max = 1.0f;
  ValueType lmin = log10(min);
  ValueType scale = 256 / (log10(max) - lmin);
  uint32_t* d = data;
  ConstPixelIterator p = getConstPixelIterator();
  *d++ = p.getRGB().toUIntLog(lmin,scale);
  while (p.hasNext())
  {
    ++p;
    *d++ = p.getRGB().toUIntLog(lmin,scale);
  }
  QImage img(reinterpret_cast<const uchar*>(data),static_cast<int32_t>(width),static_cast<int32_t>(height),QImage::Format_RGB32);
  return img;
}

QImage FitsImage::toQImageSqrt(ValueType min, ValueType max) const
{
  uint32_t* data = new uint32_t[width*height];
  if (min < 0) min = 0.0f;
  if (max < 0) max = 1.0f;
  ValueType lmin = sqrt(min);
  ValueType scale = 256 / (sqrt(max) - lmin);
  uint32_t* d = data;
  ConstPixelIterator p = getConstPixelIterator();
  *d++ = p.getRGB().toUIntSqrt(lmin,scale);
  while (p.hasNext())
  {
    ++p;
    *d++ = p.getRGB().toUIntSqrt(lmin,scale);
  }
  QImage img(reinterpret_cast<const uchar*>(data),static_cast<int32_t>(width),static_cast<int32_t>(height),QImage::Format_RGB32);
  return img;
}

QImage FitsImage::toQImageSine(ValueType min, ValueType max) const
{
  uint32_t* data = new uint32_t[width*height];
  ValueType scale = M_PI_2 / (max - min);
  uint32_t* d = data;
  ConstPixelIterator p = getConstPixelIterator();
  *d++ = p.getRGB().toUIntSine(min,scale);
  while (p.hasNext())
  {
    ++p;
    *d++ = p.getRGB().toUIntSine(min,scale);
  }
  QImage img(reinterpret_cast<const uchar*>(data),static_cast<int32_t>(width),static_cast<int32_t>(height),QImage::Format_RGB32);
  return img;
}

