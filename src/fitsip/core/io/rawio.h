/********************************************************************************
 *                                                                              *
 * FitsIP - DSLR raw image format reader                                        *
 *                                                                              *
 * modified: 2025-11-03                                                         *
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

#ifndef RAWIO_H
#define RAWIO_H

#include "iohandler.h"
#include <libraw/libraw.h>

class RawIO: public IOHandler
{
public:
  RawIO();
  ~RawIO() override;

  bool handlesFile(const QString& filename);

  virtual std::vector<std::shared_ptr<FitsObject>> read(QString filename) override;

  virtual bool write(QString filename, FitsObject* obj) override;

  static const char* FILENAME_FILTER;

private:
  template<typename T> void copyGray(FitsImage* img, libraw_processed_image_t* src);
  template<typename T> void copyColor(FitsImage* img, libraw_processed_image_t* src);

};

#endif // RAWIO_H
