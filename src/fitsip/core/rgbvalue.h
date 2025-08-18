/********************************************************************************
 *                                                                              *
 * FitsIP - class to represent na RGB value                                     *
 *                                                                              *
 * modified: 2023-01-07                                                         *
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

#ifndef RGBVALUE_H
#define RGBVALUE_H

#include "fitstypes.h"
#include <cstdint>
#include <cmath>
#include <algorithm>

#define RGB_INDEX_MIN 0
#define RGB_RED       0
#define RGB_GREEN     1
#define RGB_BLUE      2
#define RGB_SUM       3
#define RGB_INDEX_MAX 4

class RGBValue {
 public:

  inline RGBValue(void);
  inline RGBValue(ValueType gray);
  inline RGBValue(ValueType r, ValueType g, ValueType b);
  inline RGBValue(uint32_t rgb);

  inline ValueType red(void) const;
  inline ValueType green(void) const;
  inline ValueType blue(void) const;

  void getHSI(ValueType*, ValueType*, ValueType*);
  void setHSI(ValueType, ValueType, ValueType);
  void setUpperLimit(ValueType);
  void setLowerLimit(ValueType);

  inline ValueType gray(void) const;

  inline uint32_t toUInt() const;

  /**
   * @brief Convert the rgb value to a 32bit unsigned int value for ARGB images using a linear scale.
   * @param min the minimum value corresponding to black
   * @param scale the scale
   * @return the ARGB value
   */
  inline uint32_t toUInt(ValueType min, ValueType scale) const;

  /**
   * @brief Convert the rgb value to a 32bit unsigned int value for ARGB images using a log10 scale.
   *
   * Note: the min value and the scale must already be prepared for log scaling. If mi and ma are the
   * images min and max pixel value, than the parameter min = log10(mi) and the scale is
   * log10(ma) - log10(mi).
   * @param min the minimum value corresponding to black
   * @param scale the scale
   * @return the ARGB value
   */
  inline uint32_t toUIntLog(ValueType min, ValueType scale) const;

  /**
   * @brief Convert the rgb value to a 32bit unsigned int value for ARGB images using a sqrt scale.
   *
   * Note: the min value and the scale must already be prepared for sqrt scaling. If mi and ma are the
   * images min and max pixel value, than the parameter min = sqrt(mi) and the scale is
   * sqrt(ma) - sqrt(mi).
   * @param min the minimum value corresponding to black
   * @param scale the scale
   * @return the ARGB value
   */
  inline uint32_t toUIntSqrt(ValueType min, ValueType scale) const;

  /**
   * @brief Convert the rgb value to a 32bit unsigned int value for ARGB images using a sine scale.
   *
   * Note: the min value and the scale must already be prepared for sqrt scaling. If mi and ma are the
   * images min and max pixel value, than the parameter min = mi and the scale is
   * PI/2 / (ma - mi).
   * @param min the minimum value corresponding to black
   * @param scale the scale
   * @return the ARGB value
   */
  inline uint32_t toUIntSine(ValueType min, ValueType scale) const;

  inline ValueType min() const;

  inline ValueType max() const;

  /* Return the color value according to the index @a i. This allows easy use of RGBValue in loops. */
  ValueType operator[](int i) const;

  inline RGBValue operator+=(const RGBValue&);
  inline RGBValue operator-=(const RGBValue&);
  inline RGBValue operator/=(ValueType);
  inline RGBValue operator*=(ValueType);

  inline friend RGBValue operator+(const RGBValue&, const RGBValue&);
  inline friend RGBValue operator-(const RGBValue&, const RGBValue&);
  inline friend RGBValue operator/(const RGBValue&, ValueType);
  inline friend RGBValue operator*(const RGBValue&, ValueType);
  inline friend RGBValue operator*(ValueType, const RGBValue&);

  static ValueType gray(ValueType, ValueType, ValueType);

  ValueType r;
  ValueType g;
  ValueType b;
};

inline RGBValue::RGBValue(void):
  r(0),g(0),b(0)
{
}

inline RGBValue::RGBValue(ValueType gray):
  r(gray),g(gray),b(gray)
{
}

inline RGBValue::RGBValue(ValueType r, ValueType g, ValueType b):
  r(r),g(g),b(b)
{
}

inline RGBValue::RGBValue(uint32_t rgb)
{
  r = (rgb >> 16) & 0xFF;
  g = (rgb >> 8) & 0xFF;
  b = rgb & 0xFF;
}


inline ValueType RGBValue::red(void) const
{
  return r;
}

inline ValueType RGBValue::green(void) const
{
  return g;
}

inline ValueType RGBValue::blue(void) const
{
  return b;
}

inline ValueType RGBValue::gray(void) const
{
//  return (r * 11 + g * 16 + b * 5) / 32;
  return 0.2989 * r + 0.5870 * g + 0.1140 * b;
//  return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

inline uint32_t RGBValue::toUInt() const
{
  return 0xFF000000 + ((static_cast<uint32_t>(r) & 0xFF) << 16)  + ((static_cast<uint32_t>(g) & 0xFF) << 8)  + (static_cast<uint32_t>(b) & 0xFF);
}

inline uint32_t RGBValue::toUInt(ValueType min, ValueType scale) const
{
  int32_t ir = static_cast<int32_t>((r - min) * scale);
  if (ir < 0) ir = 0;
  if (ir > 255) ir = 255;
  int32_t ig = static_cast<int32_t>((g - min) * scale);
  if (ig < 0) ig = 0;
  if (ig > 255) ig = 255;
  int32_t ib = static_cast<int32_t>((b - min) * scale);
  if (ib < 0) ib = 0;
  if (ib > 255) ib = 255;
  return 0xFF000000 + ((static_cast<uint32_t>(ir) & 0xFF) << 16)  + ((static_cast<uint32_t>(ig) & 0xFF) << 8)  + (static_cast<uint32_t>(ib) & 0xFF);
}

inline uint32_t RGBValue::toUIntLog(ValueType min, ValueType scale) const
{
  int32_t ir = r <= 0 ? 0 : static_cast<int32_t>((log10(r) - min) * scale);
  if (ir < 0) ir = 0;
  if (ir > 255) ir = 255;
  int32_t ig = g <= 0 ? 0 : static_cast<int32_t>((log10(g) - min) * scale);
  if (ig < 0) ig = 0;
  if (ig > 255) ig = 255;
  int32_t ib = b <= 0 ? 0 : static_cast<int32_t>((log10(b) - min) * scale);
  if (ib < 0) ib = 0;
  if (ib > 255) ib = 255;
  return 0xFF000000 + ((static_cast<uint32_t>(ir) & 0xFF) << 16)  + ((static_cast<uint32_t>(ig) & 0xFF) << 8)  + (static_cast<uint32_t>(ib) & 0xFF);
}

inline uint32_t RGBValue::toUIntSqrt(ValueType min, ValueType scale) const
{
  int32_t ir = r < 0 ? 0 : static_cast<int32_t>((sqrt(r) - min) * scale);
  if (ir < 0) ir = 0;
  if (ir > 255) ir = 255;
  int32_t ig = g < 0 ? 0 : static_cast<int32_t>((sqrt(g) - min) * scale);
  if (ig < 0) ig = 0;
  if (ig > 255) ig = 255;
  int32_t ib = b < 0 ? 0 : static_cast<int32_t>((sqrt(b) - min) * scale);
  if (ib < 0) ib = 0;
  if (ib > 255) ib = 255;
  return 0xFF000000 + ((static_cast<uint32_t>(ir) & 0xFF) << 16)  + ((static_cast<uint32_t>(ig) & 0xFF) << 8)  + (static_cast<uint32_t>(ib) & 0xFF);
}

inline uint32_t RGBValue::toUIntSine(ValueType min, ValueType scale) const
{
  int32_t ir = r < 0 ? 0 : static_cast<int32_t>(sin((r-min)*scale)*255);
  if (ir < 0) ir = 0;
  if (ir > 255) ir = 255;
  int32_t ig = g < 0 ? 0 : static_cast<int32_t>(sin((g-min)*scale)*255);
  if (ig < 0) ig = 0;
  if (ig > 255) ig = 255;
  int32_t ib = b < 0 ? 0 : static_cast<int32_t>(sin((b-min)*scale)*255);
  if (ib < 0) ib = 0;
  if (ib > 255) ib = 255;
  return 0xFF000000 + ((static_cast<uint32_t>(ir) & 0xFF) << 16)  + ((static_cast<uint32_t>(ig) & 0xFF) << 8)  + (static_cast<uint32_t>(ib) & 0xFF);
}

inline ValueType RGBValue::min() const
{
  return std::min({r,g,b});
}

inline ValueType RGBValue::max() const
{
  return std::max({r,g,b});
}

inline RGBValue RGBValue::operator+=(const RGBValue &rgb1)
{
  r += rgb1.r;
  g += rgb1.g;
  b += rgb1.b;
  return *this;
}

inline RGBValue RGBValue::operator-=(const RGBValue &rgb1)
{
  r -= rgb1.r;
  g -= rgb1.g;
  b -= rgb1.b;
  return *this;
}

inline RGBValue RGBValue::operator/=(ValueType c)
{
  r /= c;
  g /= c;
  b /= c;
  return *this;
}

inline RGBValue RGBValue::operator*=(ValueType c)
{
  r *= c;
  g *= c;
  b *= c;
  return *this;
}

inline RGBValue operator+(const RGBValue &rgb1, const RGBValue &rgb2)
{
  RGBValue rgb;

  rgb.r = rgb1.r + rgb2.r;
  rgb.g = rgb1.g + rgb2.g;
  rgb.b = rgb1.b + rgb2.b;
  return rgb;
}

inline RGBValue operator-(const RGBValue &rgb1, const RGBValue &rgb2)
{
  RGBValue rgb;

  rgb.r = rgb1.r - rgb2.r;
  rgb.g = rgb1.g - rgb2.g;
  rgb.b = rgb1.b - rgb2.b;
  return rgb;
}

inline RGBValue operator/(const RGBValue &rgb1, ValueType c)
{
  RGBValue rgb;

  rgb.r = rgb1.r / c;
  rgb.g = rgb1.g / c;
  rgb.b = rgb1.b / c;
  return rgb;
}

inline RGBValue operator*(const RGBValue &rgb1, ValueType c)
{
  RGBValue rgb;

  rgb.r = rgb1.r * c;
  rgb.g = rgb1.g * c;
  rgb.b = rgb1.b * c;
  return rgb;
}

inline RGBValue operator*(ValueType c, const RGBValue &rgb1)
{
  RGBValue rgb;

  rgb.r = rgb1.r * c;
  rgb.g = rgb1.g * c;
  rgb.b = rgb1.b * c;
  return rgb;
}


#endif // RGBVALUE_H
