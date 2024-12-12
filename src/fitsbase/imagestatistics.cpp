/********************************************************************************
 *                                                                              *
 * FitsIP - image statistics                                                    *
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

#include "imagestatistics.h"
#include "fitsimage.h"

LayerStatistics::LayerStatistics():
  minValue(std::numeric_limits<ValueType>::max()),
  maxValue(-std::numeric_limits<ValueType>::max()),
  meanValue(0),
  stddev(0)
{
}



ImageStatistics::ImageStatistics()
{
}

ImageStatistics::ImageStatistics(const FitsImage& img, QRect rect)
{
  calculate(img,rect);
}

QRect ImageStatistics::getAOI() const
{
  return aoi;
}

const LayerStatistics& ImageStatistics::getGlobalStatistics() const
{
  return global;
}

const std::vector<LayerStatistics>& ImageStatistics::getLayerStatistics() const
{
  return layers;
}






void ImageStatistics::calculate(const FitsImage &img, QRect rect)
{
  if (rect.isEmpty())
  {
    rect = QRect(0,0,img.getWidth(),img.getHeight());
  }
  std::vector<ValueType> sum;
  std::vector<ValueType> sum2;
  int32_t n = 0;
  layers.clear();
  for (uint32_t d=0;d<img.getDepth();d++)
  {
    layers.push_back(LayerStatistics());
    sum.push_back(0);
    sum2.push_back(0);
  }
  ValueType gsum = 0;
  ValueType gsum2 = 0;
  for (int32_t y=0;y<rect.height();y++)
  {
    ConstPixelIterator p = img.getConstPixelIterator(rect.x(),y);
    for (int32_t x=0;x<rect.width();x++)
    {
      for (uint32_t d=0;d<img.getDepth();d++)
      {
        global.maxValue = std::max(global.maxValue,p[d]);
        global.minValue = std::min(global.minValue,p[d]);
        layers[d].maxValue = std::max(layers[d].maxValue,p[d]);
        layers[d].minValue = std::min(layers[d].minValue,p[d]);
        sum[d] += p[d];
        sum2[d] += p[d] * p[d];
      }
      ValueType v = p.getAbs();
      global.maxValue = std::max(global.maxValue,v);
      global.minValue = std::min(global.minValue,v);
      gsum += v;
      gsum2 += v * v;
      n++;
      ++p;
    }
  }
  for (uint32_t d=0;d<layers.size();d++)
  {
    layers[d].meanValue = sum[d] / n;
    layers[d].stddev = sqrt((n*sum2[d]-sum[d]*sum[d])/(n*static_cast<ValueType>(n-1)));
  }
  global.meanValue = gsum / n;
  global.stddev = sqrt((n*gsum2-gsum*gsum)/(n*static_cast<ValueType>(n-1)));
  aoi = rect;
}

