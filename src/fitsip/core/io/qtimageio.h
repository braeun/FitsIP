/********************************************************************************
 *                                                                              *
 * FitsIP - reader and writer for image formats handled by Qt                   *
 *                                                                              *
 * modified: 2022-11-22                                                         *
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

#ifndef QTIMAGEIO_H
#define QTIMAGEIO_H

#include <fitsip/core/fitsconfig.h>
#include "iohandler.h"
#include "../imagemetadata.h"

class QtImageIO: public IOHandler
{
public:
  QtImageIO();
  ~QtImageIO() override;

  virtual std::shared_ptr<FitsImage> read(QString filename) override;

  virtual bool write(QString filename, std::shared_ptr<FitsImage> img) override;

  static const char* FILENAME_FILTER;

private:
  void readMetadata(QString filename, ImageMetadata* data);
  void writeMetadata(QString filename, const ImageMetadata& data);
#ifdef HAVE_EXIV2
  void readExif(QString filename, ImageMetadata* data);
  void writeExif(QString filename, std::shared_ptr<FitsImage> img);
#endif
};

#endif // QTIMAGEIO_H
