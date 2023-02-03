/********************************************************************************
 *                                                                              *
 * FitsIP - factory for point-spread-functions                                  *
 *                                                                              *
 * modified: 2023-02-03                                                         *
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

#include "psffactory.h"
#include "gaussianpsf.h"
#include "cosinebellpsf.h"

std::unique_ptr<PSFFactory> PSFFactory::instance;

PSFFactory::PSFFactory()
{
  list.push_back(std::shared_ptr<PSF>(new CosineBellPSF()));
  list.push_back(std::shared_ptr<PSF>(new GaussianPSF()));
}

const PSF* PSFFactory::getPSF(const QString &name) const
{
  for (const auto& psf : list)
  {
    if (psf->getName() == name) return psf.get();
  }
  return nullptr;
}

const std::vector<std::shared_ptr<PSF>>& PSFFactory::getList() const
{
  return list;
}

PSFFactory* PSFFactory::getInstance()
{
  if (!instance) instance = std::make_unique<PSFFactory>();
  return instance.get();
}
