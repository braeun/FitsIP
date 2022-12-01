/********************************************************************************
 *                                                                              *
 * FitsIP - factory for I/O handlers                                            *
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

#include "iofactory.h"
#include "astroimageio.h"
#include "cookbookio.h"
#include "fitsio.h"
#include "qtimageio.h"
#ifdef HAVE_LIBRAW
#include "rawio.h"
#endif
#include <QByteArray>
#include <QDebug>
#include <QFileInfo>
#include <QTextStream>

IOFactory* IOFactory::instance = nullptr;

IOFactory::IOFactory():QObject()
{
}

IOFactory::~IOFactory()
{
}

IOFactory* IOFactory::getInstance()
{
  if (!instance) instance = new IOFactory;
  return instance;
}

IOHandler* IOFactory::getHandler(QString filename)
{
  QString suffix = QFileInfo(filename).suffix().toLower();
  if (handlers.find(suffix) == handlers.end())
  {
    IOHandler* handler = createHandler(filename);
    if (!handler) return handler;
    handlers.insert(std::make_pair(suffix,handler));
    connect(handler,&IOHandler::logProfilerResult,this,&IOFactory::logProfilerResult);
  }
  return handlers[suffix];
}

QString IOFactory::getReadFilters() const
{
  QByteArray a;
  QTextStream s(&a);
  s << "All (*)";
  s << ";;" << FitsIO::FILENAME_FILTER;
  s << ";;" << QtImageIO::FILENAME_FILTER;
#ifdef HAVE_LIBRAW
  s << ";;" << RawIO::FILENAME_FILTER;
#endif
  s << ";;" << AstroImageIO::FILENAME_FILTER;
  s << ";;" << CookbookIO::FILENAME_FILTER;
  s.flush();
  return a;
}

QString IOFactory::getWriteFilters() const
{
  QByteArray a;
  QTextStream s(&a);
  s << FitsIO::FILENAME_FILTER;
  s << ";;" << QtImageIO::FILENAME_FILTER;
  s.flush();
  return a;
}



IOHandler* IOFactory::createHandler(QString filename)
{
  QString suffix = QFileInfo(filename).suffix().toLower();
  if (suffix == "gz")
  {
    QString s = QFileInfo(filename).completeSuffix().toLower();
    if (s == "fts.gz" || s == "fit.gz" || s == "fits.gz") return new FitsIO;
  }
  if (suffix == "fts" || suffix == "fit" || suffix == "fits") return new FitsIO;
  if (suffix == "png") return new QtImageIO;
  if (suffix == "jpg" || suffix == "jpeg") return new QtImageIO;
  if (suffix == "bmp") return new QtImageIO;
  if (suffix == "pa" || suffix == "p1") return new CookbookIO;
  if (suffix == "aimg") return new AstroImageIO;
#ifdef HAVE_LIBRAW
  auto rawio = new RawIO;
  if (rawio->handlesFile(filename)) return rawio;
#endif
  return nullptr;
}

