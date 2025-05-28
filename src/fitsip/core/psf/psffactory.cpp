/********************************************************************************
 *                                                                              *
 * FitsIP - factory for point-spread-functions                                  *
 *                                                                              *
 * modified: 2025-02-28                                                         *
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
#include "imagepsf.h"
#include "../histogram.h"
#include "../settings.h"
#include "../io/iofactory.h"
#include <QDir>
#include <iostream>

std::unique_ptr<PSFFactory> PSFFactory::instance;

PSFFactory::PSFFactory()
{
  rebuild();
}

void PSFFactory::rebuild()
{
  list.clear();
  list.push_back(std::shared_ptr<PSF>(new CosineBellPSF()));
  list.push_back(std::shared_ptr<PSF>(new GaussianPSF()));
  QDir psfdir(Settings().getInternalPSFDirectory());
  std::cout << psfdir.absolutePath().toStdString() << std::endl;
  for (const QFileInfo& entry : psfdir.entryInfoList(QStringList("*"),QDir::NoDotAndDotDot|QDir::Files))
  {
    std::cout << "File: " << entry.absoluteFilePath().toStdString() << std::endl;
    list.push_back(std::make_shared<ImagePSF>(entry.absoluteFilePath()));
  }
}

const PSF* PSFFactory::getPSF(const QString &name) const
{
  for (const auto& psf : list)
  {
    if (psf->getName() == name)
    {
      psf->init();
      return psf.get();
    }
  }
  return nullptr;
}

const std::vector<std::shared_ptr<PSF>>& PSFFactory::getList() const
{
  return list;
}

bool PSFFactory::addPSF(const std::shared_ptr<FitsImage>& img, const QString filename)
{
  QFileInfo file(QDir(Settings().getInternalPSFDirectory()).absoluteFilePath(QFileInfo(filename).baseName()+".fts"));
  IOHandler* handler = IOFactory::getInstance()->getHandler(file.fileName());
  if (!handler) return false;
  auto tmp = img->toGray();
  Histogram hist;
  hist.build(tmp.get());
  *tmp /= hist.getBrightness();
  bool ret =handler->write(file.absoluteFilePath(),tmp);
  if (ret)
  {
    list.push_back(std::make_shared<ImagePSF>(file.absoluteFilePath()));
  }
  return ret;
}



PSFFactory* PSFFactory::getInstance()
{
  if (!instance) instance = std::make_unique<PSFFactory>();
  return instance.get();
}



