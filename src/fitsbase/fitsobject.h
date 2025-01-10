/********************************************************************************
 *                                                                              *
 * FitsIP - file object containing the image and other data                     *
 *                                                                              *
 * modified: 2025-01-10                                                         *
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

#ifndef FITSOBJECT_H
#define FITSOBJECT_H

#include "fitsimage.h"
#include "histogram.h"
#include "profile.h"
#include "undostack.h"
#include <memory>
#include <QString>

class FitsObject
{
public:
  FitsObject(QString filename, std::shared_ptr<FitsImage> img);

  int32_t getId() const;

  QString getFilename() const;

  std::shared_ptr<FitsImage> getImage();

  std::shared_ptr<Histogram> getHistogram();

  void updateHistogram();

  void setXProfile(const Profile& p);

  const Profile& getXProfile() const;

  void setYProfile(const Profile& p);

  const Profile& getYProfile() const;

  bool save(QString filename);

  void pushUndo();

  void popUndo();

  bool isUndoAvailable() const;

private:
  const int32_t id;
  QString filename;
  std::shared_ptr<FitsImage> image;
  std::shared_ptr<Histogram> histogram;
  Profile xprofile;
  Profile yprofile;
  UndoStack undostack;

  static int32_t idCounter;
};

#endif // FILEOBJECT_H
