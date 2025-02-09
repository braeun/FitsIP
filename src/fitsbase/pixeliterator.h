/********************************************************************************
 *                                                                              *
 * FitsIP - iterator to iterate over an images pixels                           *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#ifndef PIXELITERATOR_H
#define PIXELITERATOR_H

#include "fitstypes.h"
#include "rgbvalue.h"
#include <cstdint>
#include <cmath>
#include <vector>
#include <stdexcept>

/**
 * @brief Pixel view of the FITS image
 */
class ConstPixelIterator {
 public:
  inline ConstPixelIterator(int size, const std::vector<const ValueType*>& layers);
//  inline ConstPixelIterator(const ConstPixelIterator&);

  inline bool hasNext() const;
  inline bool hasPrevious() const;
  inline int getPos() const;
  inline ValueType getAbs() const;
  inline RGBValue getRGB() const;
  inline ValueType min() const;
  inline ValueType max() const;
  inline ValueType operator[](int layer) const;
  inline ConstPixelIterator operator++(void);
  inline ConstPixelIterator operator++(int);
  inline ConstPixelIterator operator--(void);
  inline ConstPixelIterator operator--(int);
  inline ConstPixelIterator operator+(int d);
  inline ConstPixelIterator operator-(int d);
  inline ConstPixelIterator& operator+=(int d);
  inline ConstPixelIterator& operator-=(int d);

private:
  bool color;
  int size;
  int index;
  std::vector<const ValueType*> layers;
};

inline ConstPixelIterator::ConstPixelIterator(int size, const std::vector<const ValueType*>& layers):
  color(false),
  size(size),
  index(0),
  layers(layers)
{
}

//inline ConstPixelIterator::ConstPixelIterator(const ConstPixelIterator &p):
//  color(p.color),
//  size(p.size),
//  index(p.index),
//  layers(p.layers)
//{
//}

inline ValueType ConstPixelIterator::operator[](int layer) const
{
  if (layer >= layers.size()) throw std::invalid_argument("no such layer");
  return *layers[layer];
}

inline ConstPixelIterator ConstPixelIterator::operator++(void)
{
  if (index < size-1)
  {
    for (const ValueType*& p : layers) p++;
    index++;
  }
  return *this;
}

inline ConstPixelIterator ConstPixelIterator::operator++(int)
{
  ConstPixelIterator it = *this;
  if (index < size-1)
  {
    for (const ValueType*& p : layers) p++;
    index++;
  }
  return it;
}

inline ConstPixelIterator ConstPixelIterator::operator--(void)
{
  if (index > 0)
  {
    for (const ValueType*& p : layers) p--;
    index--;
  }
  return *this;
}

inline ConstPixelIterator ConstPixelIterator::operator--(int)
{
  ConstPixelIterator it = *this;
  if (index > 0)
  {
    for (const ValueType*& p : layers) p--;
    index--;
  }
  return it;
}

inline ConstPixelIterator ConstPixelIterator::operator+(int d)
{
  ConstPixelIterator it = *this;
  it += d;
  return it;
}

inline ConstPixelIterator ConstPixelIterator::operator-(int d)
{
  ConstPixelIterator it = *this;
  it -= d;
  return it;
}

inline ConstPixelIterator& ConstPixelIterator::operator+=(int d)
{
  if (index + d >= size) d = size - index - 1;
  for (const ValueType*& p : layers) p += d;
  index += d;
  return *this;
}

inline ConstPixelIterator& ConstPixelIterator::operator-=(int d)
{
  if (d > index) d = index;
  for (const ValueType*& p : layers) p -= d;
  index -= d;
  return *this;
}

inline bool ConstPixelIterator::hasNext() const
{
  return index < size - 1;
}

inline bool ConstPixelIterator::hasPrevious() const
{
  return index > 0;
}

inline int ConstPixelIterator::getPos() const
{
  return index;
}

inline ValueType ConstPixelIterator::getAbs() const
{
  switch (layers.size())
  {
    case 1:
      return *layers[0];
    case 2:
      // TODO: it seems the sqrt works on double???
      return hypot(*layers[0],*layers[1]);
    case 3:
      return (*layers[0] * 11 + *layers[1] * 16 + *layers[2] * 5) / 32;
  }
  ValueType v = 0;
  for (const ValueType* p : layers) v += *p * *p;
  return sqrt(v);
}

inline RGBValue ConstPixelIterator::getRGB(void) const
{
  switch (layers.size())
  {
    case 1:
      return RGBValue(*layers[0]);
    case 2:
      // TODO: it seems the sqrt works on double???
      return RGBValue(static_cast<ValueType>(hypot(*layers[0],*layers[1])));
    case 3:
      return RGBValue(*layers[0],*layers[1],*layers[2]);
  }
  return RGBValue(0,0,0);
}

inline ValueType ConstPixelIterator::min() const
{
  if (layers.size() == 1) return *layers[0];
  ValueType v = std::min(*layers[0],*layers[1]);
  for (size_t i=2;i<layers.size();i++) v = std::min(v,*layers[i]);
  return v;
}

inline ValueType ConstPixelIterator::max() const
{
  if (layers.size() == 1) return *layers[0];
  ValueType v = std::max(*layers[0],*layers[1]);
  for (size_t i=2;i<layers.size();i++) v = std::max(v,*layers[i]);
  return v;
}






/**
 * @brief Pixel view of the FITS image
 */
class PixelIterator {
 public:
  inline PixelIterator(int size, const std::vector<ValueType*>& layers);
//  inline PixelIterator(const PixelIterator&);

  inline bool hasNext() const;
  inline bool hasPrevious() const;
  inline int getPos() const;
  inline ValueType getAbs() const;
  inline RGBValue getRGB() const;
  inline ValueType min() const;
  inline ValueType max() const;
  inline void clear();
  inline void scale(ValueType v);
  inline void set(ValueType v);
  inline ValueType operator[](int layer) const;
  inline ValueType& operator[](int layer);
  inline PixelIterator operator++(void);
  inline PixelIterator operator++(int);
  inline PixelIterator operator--(void);
  inline PixelIterator operator--(int);
  inline PixelIterator operator+(int d);
  inline PixelIterator operator-(int d);
  inline PixelIterator& operator+=(int d);
  inline PixelIterator& operator-=(int d);

 private:
  bool color;
  int size;
  int index;
  std::vector<ValueType*> layers;
};

inline PixelIterator::PixelIterator(int size, const std::vector<ValueType*>& layers):
  color(false),
  size(size),
  index(0),
  layers(layers)
{
}

//inline PixelIterator::PixelIterator(const PixelIterator &p):
//  color(p.color),
//  size(p.size),
//  index(p.index),
//  layers(p.layers)
//{
//}

inline ValueType PixelIterator::operator[](int layer) const
{
  if (static_cast<size_t>(layer) >= layers.size()) throw std::invalid_argument("no such layer");
  return *layers[layer];
}

inline ValueType& PixelIterator::operator[](int layer)
{
  if (static_cast<size_t>(layer) >= layers.size()) throw std::invalid_argument("no such layer");
  return *layers[layer];
}

inline PixelIterator PixelIterator::operator++(void)
{
  if (index < size-1)
  {
    for (ValueType*& p : layers) p++;
    index++;
  }
  return *this;
}

inline PixelIterator PixelIterator::operator++(int)
{
  PixelIterator it = *this;
  if (index < size-1)
  {
    for (ValueType*& p : layers) p++;
    index++;
  }
  return it;
}

inline PixelIterator PixelIterator::operator--(void)
{
  if (index > 0)
  {
    for (ValueType*& p : layers) p--;
    index--;
  }
  return *this;
}

inline PixelIterator PixelIterator::operator--(int)
{
  PixelIterator it = *this;
  if (index > 0)
  {
    for (ValueType*& p : layers) p--;
    index--;
  }
  return it;
}

inline PixelIterator PixelIterator::operator+(int d)
{
  PixelIterator it = *this;
  it += d;
  return it;
}

inline PixelIterator PixelIterator::operator-(int d)
{
  PixelIterator it = *this;
  it -= d;
  return it;
}

inline PixelIterator& PixelIterator::operator+=(int d)
{
  if (index + d >= size) d = size - index - 1;
  for (ValueType*& p : layers) p += d;
  index += d;
  return *this;
}

inline PixelIterator& PixelIterator::operator-=(int d)
{
  if (d > index) d = index;
  for (ValueType*& p : layers) p -= d;
  index -= d;
  return *this;
}

inline bool PixelIterator::hasNext() const
{
  return index < size - 1;
}

inline bool PixelIterator::hasPrevious() const
{
  return index > 0;
}

inline int PixelIterator::getPos() const
{
  return index;
}

inline ValueType PixelIterator::getAbs() const
{
  if (layers.size() == 1) return *layers[0];
  ValueType v = 0;
  for (const ValueType* p : layers) v += *p * *p;
  return sqrt(v);
}

inline RGBValue PixelIterator::getRGB(void) const
{
  switch (layers.size())
  {
    case 1:
      return RGBValue(*layers[0]);
    case 2:
      // TODO: it seems the sqrt works on double???
      return RGBValue(static_cast<ValueType>(hypot(*layers[0],*layers[1])));
    case 3:
      return RGBValue(*layers[0],*layers[1],*layers[2]);
  }
  return RGBValue(0,0,0);
}

inline ValueType PixelIterator::min() const
{
  if (layers.size() == 1) return *layers[0];
  ValueType v = std::min(*layers[0],*layers[1]);
  for (int i=2;i<layers.size();i++) v = std::min(v,*layers[i]);
  return v;
}

inline ValueType PixelIterator::max() const
{
  if (layers.size() == 1) return *layers[0];
  ValueType v = std::max(*layers[0],*layers[1]);
  for (int i=2;i<layers.size();i++) v = std::max(v,*layers[i]);
  return v;
}

inline void PixelIterator::clear()
{
  for (ValueType* p : layers) *p = 0;
}

inline void PixelIterator::scale(ValueType v)
{
  v /= getAbs();
  for (ValueType* p : layers) *p *= v;
}

inline void PixelIterator::set(ValueType v)
{
  for (ValueType* p : layers) *p = v;
}

#endif // PIXELITERATOR_H
