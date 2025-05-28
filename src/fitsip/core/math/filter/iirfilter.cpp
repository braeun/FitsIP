#include "iirfilter.h"
#include <string.h>
#include <algorithm>

IIRFilter::IIRFilter():
  a{1},
  b{0},
  dir(BACKWARD)
{
}

IIRFilter::~IIRFilter()
{
}

Filter::Direction IIRFilter::getDirection() const
{
  return dir;
}

const std::vector<ValueType>& IIRFilter::getA() const
{
  return a;
}

const std::vector<ValueType>& IIRFilter::getB() const
{
  return b;
}

void IIRFilter::normalizeGain(bool center)
{
  double sa = 0;
  for (size_t i=0;i<a.size();i++)
  {
    if (center)
    {
      sa += a[i] * ((i % 2) == 0 ? 1 : -1);
    }
    else
    {
      sa += a[i];
    }
  }
  double sb = 0;
  for (size_t i=0;i<b.size();i++)
  {
    if (center)
    {
      sb += b[i] * ((i % 2) == 0 ? 1 : -1);
    }
    else
    {
      sb += b[i];
    }
  }
  double gain = sa / (1 - sb);
  for (size_t i=0;i<a.size();i++)
  {
    a[i] /= gain;
  }
}

ValueType* IIRFilter::apply(ValueType* data, uint32_t size)
{
  if (dir == FORWARD) return applyForward(data,size);
  return applyBackward(data,size);
}

ValueType* IIRFilter::apply(ValueType* data, uint32_t width, uint32_t height)
{
  if (dir == FORWARD) return applyForward(data,width,height);
  return applyBackward(data,width,height);
}






ValueType* IIRFilter::applyForward(ValueType *data, uint32_t size)
{
  ValueType* result = new ValueType[size];
  memset(result,0,sizeof(ValueType)*size);
  for (uint32_t i=std::max(a.size(),b.size())-1;i<size;i++)
  {
    for (uint32_t j=0;j<a.size();j++)
    {
      result[i] += a[j] * data[i-j];
    }
    for (uint32_t j=1;j<b.size();j++)
    {
      result[i] += b[j] * result[i-j];
    }
  }
  return result;
}

ValueType* IIRFilter::applyForward(ValueType *data, uint32_t width, uint32_t height)
{
  ValueType* tmp = new ValueType[width*height];
  memset(tmp,0,sizeof(ValueType)*width*height);
  ValueType* row = tmp;
  ValueType* datarow = data;
  for (uint32_t r=0;r<height;r++)
  {
    for (uint32_t i=std::max(a.size(),b.size())-1;i<width;i++)
    {
      for (uint32_t j=0;j<a.size();j++)
      {
        row[i] += a[j] * datarow[i-j];
      }
      for (uint32_t j=1;j<b.size();j++)
      {
        row[i] += b[j] * row[i-j];
      }
    }
    row += width;
    datarow += width;
  }
  ValueType* result = new ValueType[width*height];
  memset(result,0,sizeof(ValueType)*width*height);
  ValueType* col = result;
  ValueType* datacol = tmp;
  for (uint32_t c=0;c<width;c++)
  {
    for (uint32_t i=std::max(a.size(),b.size())-1;i<height;i++)
    {
      for (uint32_t j=0;j<a.size();j++)
      {
        col[c+i*width] += a[j] * datacol[c+(i-j)*width];
      }
      for (uint32_t j=1;j<b.size();j++)
      {
        col[c+i*width] += b[j] * col[c+(i-j)*width];
      }
    }
    ++col;
    ++datacol;
  }
  delete [] tmp;
  return result;
}

ValueType* IIRFilter::applyBackward(ValueType *data, uint32_t size)
{
  ValueType* result = new ValueType[size];
  memset(result,0,sizeof(ValueType)*size);
  for (uint32_t i=size-std::max(a.size(),b.size())-1;i<size;i--)
  {
    for (uint32_t j=0;j<a.size();j++)
    {
      result[i] += a[j] * data[i+j];
    }
    for (uint32_t j=1;j<b.size();j++)
    {
      result[i] += b[j] * result[i+j];
    }
  }
  return result;
}

ValueType* IIRFilter::applyBackward(ValueType *data, uint32_t width, uint32_t height)
{
  ValueType* tmp = new ValueType[width*height];
  memset(tmp,0,sizeof(ValueType)*width*height);
  ValueType* row = tmp;
  ValueType* datarow = data;
  for (uint32_t r=0;r<height;r++)
  {
    for (uint32_t i=width-std::max(a.size(),b.size())-1;i<width;i--)
    {
      for (uint32_t j=0;j<a.size();j++)
      {
        row[i] += a[j] * datarow[i+j];
      }
      for (uint32_t j=1;j<b.size();j++)
      {
        row[i] += b[j] * row[i+j];
      }
    }
    row += width;
    datarow += width;
  }
  ValueType* result = new ValueType[width*height];
  memset(result,0,sizeof(ValueType)*width*height);
  ValueType* col = result;
  ValueType* datacol = tmp;
  for (uint32_t c=0;c<width;c++)
  {
    for (uint32_t i=height-std::max(a.size(),b.size())-1;i<height;i++)
    {
      for (uint32_t j=0;j<a.size();j++)
      {
        col[c+i*width] += a[j] * datacol[c+(i+j)*width];
      }
      for (uint32_t j=1;j<b.size();j++)
      {
        col[c+i*width] += b[j] * col[c+(i+j)*width];
      }
    }
    ++col;
    ++datacol;
  }
  delete [] tmp;
  return result;
}

