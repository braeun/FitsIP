/********************************************************************************
 *                                                                              *
 * FitsIP - Cookbook CCD image reader                                           *
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

#ifndef COOKBOOKIO_H
#define COOKBOOKIO_H

#include "iohandler.h"

class CookbookIO: public IOHandler
{
public:
  CookbookIO();
  ~CookbookIO() override;

  virtual std::vector<std::shared_ptr<FitsObject>> read(QString filename) override;

  virtual bool write(QString filename, FitsObject* img) override;

  static const char* FILENAME_FILTER;

private:
  std::shared_ptr<FitsImage> readPA(QString filename);
  std::shared_ptr<FitsImage> readP1(QString filename);

};

#endif // COOKBOOKIO_H
