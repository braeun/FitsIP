/********************************************************************************
 *                                                                              *
 * FitsIP - DSLR raw image format reader                                        *
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

#ifndef RAWIO_H
#define RAWIO_H

#include "iohandler.h"

class RawIO: public IOHandler
{
public:
  RawIO();
  ~RawIO() override;

  bool handlesFile(const QString& filename);

  virtual std::shared_ptr<FitsImage> read(QString filename) override;

  virtual bool write(QString filename, std::shared_ptr<FitsImage> img) override;

  static const char* FILENAME_FILTER;

};

#endif // RAWIO_H
