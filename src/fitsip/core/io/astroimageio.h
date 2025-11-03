/********************************************************************************
 *                                                                              *
 * FitsIP - astro image format reader                                           *
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

#ifndef ASTROIMAGEIO_H
#define ASTROIMAGEIO_H

#include "iohandler.h"

class QFile;
class QFileInfo;

class AstroImageIO: public IOHandler
{
public:
  AstroImageIO();
  ~AstroImageIO() override;

  virtual std::vector<std::shared_ptr<FitsObject>> read(QString filename) override;

  virtual bool write(QString filename, FitsObject* obj) override;

  static const char* FILENAME_FILTER;

private:
  std::shared_ptr<FitsImage> read_0x0100(QFile* file, const QFileInfo& info);
  std::shared_ptr<FitsImage> read_0x0200(QFile* file, const QFileInfo& info);

};

#endif // ASTROIMAGEIO_H
