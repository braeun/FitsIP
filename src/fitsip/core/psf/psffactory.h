/********************************************************************************
 *                                                                              *
 * FitsIP - factory for point-spread-functions                                  *
 *                                                                              *
 * modified: 2025-05-31                                                         *
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

#ifndef PSFFACTORY_H
#define PSFFACTORY_H

#include "psf.h"
#include <memory>
#include <vector>
#include <QString>

class PSFFactory
{
public:
  PSFFactory();

  void rebuild();

  const PSF* getPSF(const QString& name) const;

  const std::vector<std::shared_ptr<PSF>>& getList() const;

  bool addPSF(const std::shared_ptr<FitsImage>& img, const QString filename);

  bool deletePSF(const QString& name);

  bool renamePSF(const QString& name, const QString& newname);

  bool canRename(const QString& name) const;

  bool canDelete(const QString& name) const;

  static PSFFactory* getInstance();

private:

  std::vector<std::shared_ptr<PSF>> list;

  static std::unique_ptr<PSFFactory> instance;
};

#endif // PSFFACTORY_H
