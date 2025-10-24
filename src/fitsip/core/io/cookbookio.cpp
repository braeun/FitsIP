/********************************************************************************
 *                                                                              *
 * FitsIP - Cookbook CCD image reader                                           *
 *                                                                              *
 * modified: 2025-10-24                                                         *
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

#include "cookbookio.h"
#include "../fitsimage.h"
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QDebug>
#include <stdexcept>

const char* CookbookIO::FILENAME_FILTER = "Cookbook Camera (*.pa *.p1)";

CookbookIO::CookbookIO()
{
}

CookbookIO::~CookbookIO()
{
}

std::vector<std::shared_ptr<FitsImage>> CookbookIO::read(QString filename)
{
  QString suffix = QFileInfo(filename).suffix().toLower();
  if (suffix.endsWith("a")) return {readPA(filename)};
  return {readP1(filename)};
}

bool CookbookIO::write(QString /*filename*/, std::shared_ptr<FitsImage> /*img*/)
{
  throw std::runtime_error("Writing to Cookbook format not supported.");
}


std::shared_ptr<FitsImage> CookbookIO::readPA(QString filename)
{
  QFileInfo info(filename);
  profiler.start();
  auto img = std::make_shared<FitsImage>(info.baseName(),252,242,1);
  PixelIterator it = img->getPixelIterator();
  {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) throw std::runtime_error("CookbookIO: Failed to open file");
    QDataStream s(&f);
    s.skipRawData(7);
    for (int i=0;i<125;i++)
    {
      for (int j=0;j<252;j++)
      {
        uint16_t v;
        s.readRawData(reinterpret_cast<char*>(&v),sizeof(v));
        it[0] = v;
        ++it;
      }
    }
    f.close();
  }
  if (filename.endsWith("a"))
  {
    filename = filename.mid(0,filename.length()-1) + "b";
  }
  else
  {
    filename = filename.mid(0,filename.length()-1) + "B";
  }
  {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) throw std::runtime_error("CookbookIO: Failed to open file");
    QDataStream s(&f);
    s.skipRawData(7);
    for (int i=125;i<242;i++)
    {
      for (int j=0;j<252;j++)
      {
        uint16_t v;
        s.readRawData(reinterpret_cast<char*>(&v),sizeof(v));
        it[0] = v;
        ++it;
      }
    }
    f.close();
  }
  profiler.stop();
  logProfiler(img,"read");
  return img;
}

std::shared_ptr<FitsImage> CookbookIO::readP1(QString filename)
{
  QFileInfo info(filename);
  profiler.start();
  auto img = std::make_shared<FitsImage>(info.baseName(),378,242,1);
  PixelIterator it = img->getPixelIterator();
  {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) throw std::runtime_error("CookbookIO: Failed to open file");
    QDataStream s(&f);
    s.skipRawData(7);
    for (int i=0;i<81;i++)
    {
      for (int j=0;j<378;j++)
      {
        uint16_t v;
        s.readRawData(reinterpret_cast<char*>(&v),sizeof(v));
        it[0] = v;
        ++it;
      }
    }
    f.close();
  }
  filename = filename.mid(0,filename.length()-1) + "2";
  {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) throw std::runtime_error("CookbookIO: Failed to open file");
    QDataStream s(&f);
    s.skipRawData(7);
    for (int i=81;i<162;i++)
    {
      for (int j=0;j<378;j++)
      {
        uint16_t v;
        s.readRawData(reinterpret_cast<char*>(&v),sizeof(v));
        it[0] = v;
        ++it;
      }
    }
    f.close();
  }
  filename = filename.mid(0,filename.length()-1) + "3";
  {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) throw std::runtime_error("CookbookIO: Failed to open file");
    QDataStream s(&f);
    s.skipRawData(7);
    for (int i=162;i<242;i++)
    {
      for (int j=0;j<378;j++)
      {
        uint16_t v;
        s.readRawData(reinterpret_cast<char*>(&v),sizeof(v));
        it[0] = v;
        ++it;
      }
    }
    f.close();
  }
  profiler.stop();
  logProfiler(img,"read");
  return img;
}


