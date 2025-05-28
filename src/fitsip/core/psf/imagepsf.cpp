/********************************************************************************
 *                                                                              *
 * FitsIP - image based point-spread-function                                   *
 *                                                                              *
 * modified: 2025-05-24                                                         *
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

#include "imagepsf.h"
#include "../io/iofactory.h"
#include <QFileInfo>

ImagePSF::ImagePSF(QString filename):PSF(),
  filename(filename)
{
}

ImagePSF::~ImagePSF()
{
}

QString ImagePSF::getName() const
{
  return QFileInfo(filename).baseName();
}

void ImagePSF::init()
{
  if (!img)
  {
    IOHandler* handler = IOFactory::getInstance()->getHandler(filename);
    if (handler)
    {
      img = handler->read(filename);
    }
    if (!img)
    {
      img = std::make_shared<FitsImage>(getName(),50,50);
    }
  }
}

std::shared_ptr<FitsImage> ImagePSF::createPSF(int w, int h, const std::vector<ValueType>& par) const
{
  if (w < img->getWidth()) w = img->getWidth();
  if (h < img->getHeight()) h = img->getHeight();
  auto dst = std::make_shared<FitsImage>("",w,h,img->getDepth());
  int iw2 = img->getWidth() / 2;
  int iwodd = (img->getWidth() % 2);
  int ih2 = img->getHeight() / 2;
  int ihodd = (img->getHeight() % 2);
  dst->blit(img.get(),iw2,ih2,iw2+iwodd,ih2+ihodd,0,0);
  dst->blit(img.get(),0,0,iw2,ih2,dst->getWidth()-iw2,dst->getHeight()-ih2);
  dst->blit(img.get(),0,ih2,iw2,ih2+ihodd,dst->getWidth()-iw2,0);
  dst->blit(img.get(),iw2,0,iw2+iwodd,ih2,0,dst->getHeight()-ih2);
  return dst;
}

std::shared_ptr<FitsImage> ImagePSF::createPSFForDisplay(int w, int h, const std::vector<ValueType>& par) const
{
  if (w < img->getWidth()) w = img->getWidth();
  if (h < img->getHeight()) h = img->getHeight();
  auto dst = std::make_shared<FitsImage>("",w,h,img->getDepth());
  dst->blit(img.get(),0,0,img->getWidth(),img->getHeight(),(w-img->getWidth())/2,(h-img->getHeight())/2);
  return dst;
}
