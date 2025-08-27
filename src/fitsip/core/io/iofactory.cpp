/********************************************************************************
 *                                                                              *
 * FitsIP - factory for I/O handlers                                            *
 *                                                                              *
 * modified: 2025-08-19                                                         *
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
#include <QImageReader>
#include <QTextStream>

const char* IOFactory::filelist_filter = "File List (*.lst)";

const char* IOFactory::csv_filter = "Comma Separated Values (*.csv)";;

const char* IOFactory::all_files_filter = "ALL Files (*)";;

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
  // auto list = QImageReader::supportedImageFormats();
  // for (const auto& a : list)
  // {
  //   std::cout << a.data() << std::endl;
  // }
  return instance;
}

bool IOFactory::isImage(QString filename)
{
  return getHandler(filename) != nullptr;
}

bool IOFactory::isScript(QString filename)
{
  QString suffix = QFileInfo(filename).suffix().toLower();
  if (suffix == "py") return true;
  return false;
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

QString IOFactory::assertSuffix(const QString &fn, QString filter)
{
  int i = filter.lastIndexOf('(');
  if (i >= 0) filter = filter.mid(i+1);
  i = filter.lastIndexOf(')');
  if (i >= 0) filter = filter.left(i);
  QStringList suffixes = filter.split(" ",Qt::SkipEmptyParts);
  for (QString suffix : suffixes)
  {
    i = suffix.indexOf('.');
    if (i < 0) return fn;
    suffix = suffix.mid(i);
    if (fn.endsWith(suffix)) return fn;
  }
  if (suffixes.empty()) return fn;
  QString suffix = suffixes.front();
  i = suffix.indexOf('.');
  if (i < 0) return fn;
  suffix = suffix.mid(i);
  return fn + suffix;
}




IOHandler* IOFactory::createHandler(QString filename)
{
  QString suffix = QFileInfo(filename).suffix().toLower();
  if (suffix == "gz") /* support for zipped fits files */
  {
    QString s = QFileInfo(filename).completeSuffix().toLower();
    if (s == "fts.gz" || s == "fit.gz" || s == "fits.gz") return new FitsIO;
  }
  if (suffix == "fts" || suffix == "fit" || suffix == "fits") return new FitsIO;
  // if (suffix == "png") return new QtImageIO;
  // if (suffix == "jpg" || suffix == "jpeg") return new QtImageIO;
  // if (suffix == "bmp") return new QtImageIO;
  if (suffix == "pa" || suffix == "p1") return new CookbookIO;
  if (suffix == "aimg") return new AstroImageIO;
#ifdef HAVE_LIBRAW
  auto rawio = new RawIO;
  if (rawio->handlesFile(filename)) return rawio;
#endif
  auto list = QImageReader::supportedImageFormats();
  for (const auto& a : list)
  {
    if (suffix == a) return new QtImageIO;
  }
  return nullptr;
}

