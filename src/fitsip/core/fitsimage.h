/********************************************************************************
 *                                                                              *
 * FitsIP - image object                                                        *
 *                                                                              *
 * modified: 2025-08-17                                                         *
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
#include <memory>
#include <vector>
#include <valarray>


/**
 * @brief The data for a single layer in a FITS image.
 */
class Layer
{
public:
  Layer(int width, int height);
  Layer(const Layer& l);
  ~Layer();

  int getWidth() const;

  int getHeight() const;

  size_t size() const;

  void setData(std::valarray<ValueType>& d);

  ValueType* getData();

  const ValueType* getData() const;

  void blit(Layer* layer, int x, int y, int w, int h, int xd, int yd);

  inline const ValueType& operator()(int x, int y) const { return data[y*width+x];}

  inline ValueType& operator()(int x, int y) { return data[y*width+x];}

private:
  int width;
  int height;
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
  FitsImage(const QString& name, int width, int height, int depth=1);
  FitsImage(const FitsImage& img);
  FitsImage(FitsImage&& img);
  FitsImage(const QString& name, const FitsImage& img);
  FitsImage(const QString& name, std::vector<Layer*>& layers);

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

  int getWidth() const;

  int getHeight() const;

  int getDepth() const;

  /**
   * @brief Get the pixel value at the given location.
   * @param x the x position; if negative it is taken from the right
   * @param y the y position; if negative it is taken from the bottom
   * @return
   */
  Pixel getPixel(int x, int y) const;

  Pixel getPixel(const ConstPixelIterator& it) const;

  PixelIterator getPixelIterator();

  PixelIterator getPixelIterator(int x, int y);

  ConstPixelIterator getConstPixelIterator() const;

  ConstPixelIterator getConstPixelIterator(int x, int y) const;

  std::shared_ptr<Layer> getLayer(int index);

  std::shared_ptr<Layer> getLayer(int index) const;

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
   * @brief Return this image resized to the new width and height.
   *
   * If the new width / height are larger than the current one, a empty border
   * will be created. If they are smaller, the image will be cropped. In any
   * case the center of the new image corresponds to the center of the original
   * image.
   * @param w the new width
   * @param h the new height
   * @return the resized image
   */
  std::shared_ptr<FitsImage> resizedImage(int w, int h) const;

  /**
   * @brief Return this image padded to the new width and height.
   *
   * If the new width / height are larger than the current one,
   * the image will be padded towards the right and bottom.
   * If they are smaller the original width or height will be
   * used. In any case, the top-left corner of the original image
   * corresponds to the top-left corner of the new image.
   * @param w the new width
   * @param h the new height
   * @return the padded image
   */
  std::shared_ptr<FitsImage> paddedImage(int w, int h) const;

  void blit(FitsImage* src, int x, int y, int w, int h, int xd, int yd);

  /**
   * @brief Limit the intensity to the given range.
   * @param lower the lowest intensity value
   * @param upper the largest intensity value
   */
  void cut(ValueType lower, ValueType upper);

  /**
   * @brief Return a gray scale image of this one.
   * @return gray scale image
   */
  std::shared_ptr<FitsImage> toGray() const;

  void scaleIntensity(ValueType min, ValueType max);

  Pixel getBrightestPixel(const QRect& r) const;

  operator bool() const { return !isNull(); }

  FitsImage& operator+=(const FitsImage& img);

  FitsImage& operator+=(ValueType v);

  FitsImage& operator-=(const FitsImage& img);

  FitsImage& operator-=(ValueType v);

  FitsImage& operator*=(const FitsImage& img);

  FitsImage& operator*=(ValueType v);

  FitsImage& operator/=(const FitsImage& img);

  FitsImage& operator/=(ValueType v);

  FitsImage& operator=(const FitsImage&);

private:
  QImage toQImageLin(ValueType min, ValueType max) const;
  QImage toQImageLog(ValueType min, ValueType max) const;
  QImage toQImageSqrt(ValueType min, ValueType max) const;
  QImage toQImageSine(ValueType min, ValueType max) const;

  QString name;
  int width;
  int height;
  int depth;
  std::vector<std::shared_ptr<Layer>> layers;
  ImageMetadata metadata;
};

inline int FitsImage::getWidth() const
{
  return width;
}

inline int FitsImage::getHeight() const
{
  return height;
}

inline int FitsImage::getDepth() const
{
  return depth;
}

inline std::shared_ptr<Layer> FitsImage::getLayer(int index)
{
  return layers[index];
}

inline std::shared_ptr<Layer> FitsImage::getLayer(int index) const
{
  return layers[index];
}

#endif // IMAGE_H
