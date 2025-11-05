/********************************************************************************
 *                                                                              *
 * FitsIP - intensity histogram                                                 *
 *                                                                              *
 * modified: 2025-11-04                                                         *
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
#include <vector>

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

  bool isEmpty() const                  { return data[0].empty(); }
  int32_t getBinCount(void) const       { return bin; }
  ValueType getMin(void) const          { return min; }
  ValueType getMax(void) const          { return max; }
  ValueType getX(int b) const       { return min + (max - min) / bin * b; }
  int getGrayValue(int n) const     { return ((n >= 0 && n < bin) ? (data[0])[n] : 0); }
  int getRedValue(int n) const      { return ((n >= 0 && n < bin) ? (data[1])[n] : 0); }
  int getGreenValue(int n) const    { return ((n >= 0 && n < bin) ? (data[2])[n] : 0); }
  int getBlueValue(int n) const     { return ((n >= 0 && n < bin) ? (data[3])[n] : 0); }
  int getValue(int i, int n) const  { return ((n >= 0 && n < bin) ? (data[i])[n] : 0); }
  ValueType getBrightness() const   { return brightness[0]; }
  ValueType getRedBrightness() const   { return brightness[1]; }
  ValueType getGreenBrightness() const   { return brightness[2]; }
  ValueType getBlueBrightness() const   { return brightness[3]; }

  /**
   * @brief Clear histogram
   */
  void clear(void);

  /**
   * @brief Build the histogram from the specified fits image
   * @param img the image to buld the histogram.
   */
  void build(const FitsImage* img);

  /* Increment bin corresponding to pixel value v */
  inline void inc(ValueType);

  /* Increment bins corresponding to RGBValue rgb */
  inline void inc(const RGBValue&);

  bool isRGB(void) const;

  /**
   * @brief Get the sum of all entries, i.e. the number of pixels in the
   * image.
   *
   * @return sum of all entries
   */
  int getSum() const;

  /**
   * @brief Get the sum of all entries from bin 0 to maxbin inclusive.
   *
   * This sums over entries in the gray level histogram only.
   * @param maxbin the maximum bin to sum up to
   * @return sum of entries
   */
  int getSum(int maxbin) const;

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
  int sum;          /* # of entries = # of pixels in image */
  int bin;          /* number of bins */
  std::vector<int> data[4]; /* 4 histograms: [0] = gray level; [1] = red; [2] = green; [3] = blue */
  ValueType min;    /* minimum bin */
  ValueType max;    /* maximum bin */
  ValueType brightness[4]; /* integral of all pixel values: [0] = gray level; [1] = red; [2] = green; [3] = blue */
};

/*
 * Increment bin corresponding to pixel value v
 */
inline void Histogram::inc(ValueType v)
{
  int n = static_cast<int>((v - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[0])[n]++;
  brightness[0] += v;
}

/*
 * Increment bins corresponding to RGBValue rgb
 */
inline void Histogram::inc(const RGBValue &rgb)
{
  int n = static_cast<int>((rgb.r - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[1])[n]++;
  brightness[1] += rgb.r;
  n = static_cast<int>((rgb.g - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[2])[n]++;
  brightness[2] += rgb.g;
  n = static_cast<int>((rgb.b - min) / (max - min) * bin);
  if (n >= 0 && n < bin) (data[3])[n]++;
  brightness[3] += rgb.b;
}


#endif // HISTOGRAM_H
