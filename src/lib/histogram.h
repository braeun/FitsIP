/********************************************************************************
 *                                                                              *
 * FitsIP - intensity histogram                                                 *
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

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "rgbvalue.h"
#include <iostream>
#include <memory>

class FitsImage;

/**
 * @brief Intensity histogram of an image.
 */
class Histogram
{
public:
  Histogram();
  Histogram(ValueType min, ValueType max, int32_t bin);
  ~Histogram(void);

  bool isRGB(void) const                { return rgb; }
  int32_t getBinCount(void) const       { return bin; }
  ValueType getMin(void) const          { return min; }
  ValueType getMax(void) const          { return max; }
  ValueType getX(int32_t b) const       { return min + (max - min) / bin * b; }
  int getGrayValue(int32_t n) const     { return ((n >= 0 && n < bin) ? (data[0])[n] : 0); }
  int getRedValue(int32_t n) const      { return ((n >= 0 && n < bin) ? (data[1])[n] : 0); }
  int getGreenValue(int32_t n) const    { return ((n >= 0 && n < bin) ? (data[2])[n] : 0); }
  int getBlueValue(int32_t n) const     { return ((n >= 0 && n < bin) ? (data[3])[n] : 0); }
  int getValue(int i, int32_t n) const  { return ((n >= 0 && n < bin) ? (data[i])[n] : 0); }

  /**
   * @brief Clear histogram
   */
  void clear(void);

  /**
   * @brief Build the histogram from the specified fits image
   * @param img the image to buld the histogram.
   */
  void build(FitsImage* img);

  /* Increment bin corresponding to pixel value v */
  inline void inc(ValueType);

  /* Increment bins corresponding to RGBValue rgb */
  inline void inc(const RGBValue&);

  /**
   * @brief Get the sum of all entries, i.e. the number of pixels in the
   * image.
   *
   * @return sum of all entries
   */
  int32_t getSum() const;

  /**
   * @brief Get the sum of all entries from bin 0 to maxbin inclusive.
   *
   * This sums over entries in the gray level histogram only.
   * @param maxbin the maximum bin to sum up to
   * @return sum of entries
   */
  int32_t getSum(int32_t maxbin) const;

  /**
   * @brief Get the intensity level below which a certain fraction of entries lies.
   *
   * @param p fraction of entries
   * @return intensity level
   */
  ValueType getFillLevel(double p) const;

  /**
   * @brief Get the intensity average of all low intensities which make up a certain fraction of entries.
   *
   * @param p fraction of entries
   * @return intensity average
   */
  AverageResult getAverage(double p) const;

  /* Stream output operator for histogram */
  friend std::ostream &operator<<(std::ostream&, const Histogram&);

 private:
  bool rgb;         /* true if histogram also contains RGB data */
  int32_t sum;      /* # of entries = # of pixels in image */
  int32_t bin;      /* number of bins */
  int32_t *data[4]; /* 4 histograms: [0] = gray level; [1] = red; [2] = green; [3] = blue */
  ValueType min;    /* minimum bin */
  ValueType max;    /* maximum bin */
};

/*
 * Increment bin corresponding to pixel value v
 */
inline void Histogram::inc(ValueType v)
{
  int n = static_cast<int32_t>((v - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[0])[n]++;
}

/*
 * Increment bins corresponding to RGBValue rgb
 */
inline void Histogram::inc(const RGBValue &rgb)
{
  int n = static_cast<int32_t>((rgb.r - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[1])[n]++;
  n = static_cast<int32_t>((rgb.g - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[2])[n]++;
  n = static_cast<int32_t>((rgb.b - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[3])[n]++;
}


#endif // HISTOGRAM_H
