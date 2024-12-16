/********************************************************************************
 *                                                                              *
 * FitsIP - image object                                                        *
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

#ifndef FITSIMAGE_H
#define FITSIMAGE_H

#include "fitstypes.h"
#include "imagemetadata.h"
#include "pixel.h"
#include "pixeliterator.h"
#include <QImage>
#include <stdint.h>
#include <memory>
#include <vector>
#include <valarray>


/**
 * @brief The data for a single layer in a FITS image.
 */
class Layer
{
public:
  Layer(uint32_t width, uint32_t height);
  Layer(const Layer& l);
  ~Layer();

  void setData(std::valarray<ValueType>& d);

  ValueType* getData();

  const ValueType* getData() const;

private:
  uint32_t width;
  uint32_t height;
  ValueType* data;
};

inline ValueType* Layer::getData()
{
  return data;
}

inline const ValueType* Layer::getData() const
{
  return data;
}



class FitsImage
{
public:

  enum Scale { LINEAR=0, SINE, SQRT, LOG };

  FitsImage();
  FitsImage(const QString& name, uint32_t width, uint32_t height, uint32_t depth=1);
  FitsImage(const FitsImage& img);
  FitsImage(FitsImage&& img);
  FitsImage(const QString& name, const FitsImage& img);

  bool isNull() const;

  /**
   * @brief Gets the name of this image.
   * @return the display name
   */
  QString getName() const;

  /**
   * @brief Sets the name of this image.
   * @param name the name to set
   */
//  void setName(const QString& name);

  uint32_t getWidth() const;

  uint32_t getHeight() const;

  uint32_t getDepth() const;

  Pixel getPixel(uint32_t x, uint32_t y) const;

  Pixel getPixel(const ConstPixelIterator& it) const;

  PixelIterator getPixelIterator();

  PixelIterator getPixelIterator(uint32_t x, uint32_t y);

  ConstPixelIterator getConstPixelIterator() const;

  ConstPixelIterator getConstPixelIterator(uint32_t x, uint32_t y) const;

  std::shared_ptr<Layer> getLayer(uint32_t index);

  std::shared_ptr<Layer> getLayer(uint32_t index) const;

  /**
   * @brief Convert the image to a QImage suitable for display.
   * @param min minimum pixel value corresponding to black
   * @param max maximum pixel value corresponding to white
   * @param scale scaling method
   * @return the QImage
   */
  QImage toQImage(ValueType min, ValueType max, Scale scale) const;

  const ImageMetadata& getMetadata() const;

  void setMetadata(const ImageMetadata& data);

  /**
   * @brief Add the message to the history field in the metadata
   * @param msg the message to log
   */
  void log(QString msg);

  /**
   * @brief Check if a given image is compatible with this one.
   *
   * Two images are considered compatible if they have the same width, height
   * and depth.
   * @param img the image to check
   * @return true if it is compatible.
   */
  bool isCompatible(const FitsImage& img) const;

  /**
   * @brief Return the overlap between the image and a given rectangle
   * @param r the rectangle
   * @return the overlapping rectangle
   */
  QRect getOverlap(const QRect& r) const;

  /**
   * @brief Return a new sub image
   * @param r the rectangle for the sub image
   * @return the new sub image
   */
  std::shared_ptr<FitsImage> subImage(const QRect& r) const;

  /**
   * @brief Return a gray scale image of this one.
   * @return gray scale image
   */
  std::shared_ptr<FitsImage> toGray();

  Pixel getBrightestPixel(const QRect& r) const;

  FitsImage& operator+=(const FitsImage& img);

  FitsImage& operator+=(ValueType v);

  FitsImage& operator-=(const FitsImage& img);

  FitsImage& operator-=(ValueType v);

  FitsImage& operator*=(const FitsImage& img);

  FitsImage& operator*=(ValueType v);

  FitsImage& operator/=(const FitsImage& img);

  FitsImage& operator/=(ValueType v);

  FitsImage& operator=(const FitsImage&);

  uint32_t preFFTWidth;
  uint32_t preFFTHeight;

private:
  QImage toQImageLin(ValueType min, ValueType max) const;
  QImage toQImageLog(ValueType min, ValueType max) const;
  QImage toQImageSqrt(ValueType min, ValueType max) const;
  QImage toQImageSine(ValueType min, ValueType max) const;

  QString name;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  std::vector<std::shared_ptr<Layer>> layers;
  ImageMetadata metadata;
};

inline uint32_t FitsImage::getWidth() const
{
  return width;
}

inline uint32_t FitsImage::getHeight() const
{
  return height;
}

inline uint32_t FitsImage::getDepth() const
{
  return depth;
}

inline std::shared_ptr<Layer> FitsImage::getLayer(uint32_t index)
{
  return layers[index];
}

inline std::shared_ptr<Layer> FitsImage::getLayer(uint32_t index) const
{
  return layers[index];
}

#endif // IMAGE_H
