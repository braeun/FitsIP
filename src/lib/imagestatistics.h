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

#ifndef IMAGESTATISTICS_H
#define IMAGESTATISTICS_H

#include <fitstypes.h>
#include <QRect>
#include <vector>
#include <memory>

class FitsImage;

class LayerStatistics
{
public:
  LayerStatistics();

  ValueType minValue;
  ValueType maxValue;
  ValueType meanValue;
  ValueType stddev;
};

class ImageStatistics
{
public:
  ImageStatistics();
  ImageStatistics(const FitsImage& img, QRect rect=QRect());

  QRect getAOI() const;

  const LayerStatistics& getGlobalStatistics() const;

  const std::vector<LayerStatistics>& getLayerStatistics() const;

private:
  void calculate(const FitsImage& img, QRect rect);

  QRect aoi;
  LayerStatistics global;
  std::vector<LayerStatistics> layers;
};


#endif // IMAGESTATISTICS_H
