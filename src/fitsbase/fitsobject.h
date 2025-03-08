﻿/********************************************************************************
 *                                                                              *
 * FitsIP - file object containing the image and other data                     *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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
#include "pixellist.h"
#include "profile.h"
#include "starlist.h"
#include "undostack.h"
#include <memory>
#include <string>

/**
 * @brief The FitsObject class contains an object in the FitsIP application,
 *        which encapsulates the image and additional data.
 */
class FitsObject
{
public:
  FitsObject(std::shared_ptr<FitsImage> img, const QString& filename="");
  FitsObject(std::shared_ptr<FitsImage> img, const std::string& filename);
  FitsObject(const FitsObject& obj) = delete;
  ~FitsObject();

  FitsObject& operator=(const FitsObject& obj) = delete;

  int getId() const;

  QString getName() const;

  QString getFilename() const;

  std::shared_ptr<FitsImage> getImage() const;

  void setImage(const std::shared_ptr<FitsImage>& img);

  QRect getAOI() const;

  void setAOI(const QRect& r);

  const Histogram& getHistogram(bool update=false);

  void updateHistogram();

  void setXProfile(const Profile& p);

  const Profile& getXProfile() const;

  void setYProfile(const Profile& p);

  const Profile& getYProfile() const;

  PixelList* getPixelList() const;

  StarList* getStarList() const;

  bool save(const QString& filename);

  bool save(const std::string& filename);

  void pushUndo();

  void popUndo();

  bool isUndoAvailable() const;

  /**
   * @brief Create a clone of this fits object with a new id.
   *
   * Note: the undo stack is not cloned.
   * @param filename the new filename
   * @return shared pointer to the new cloned fits object
   */
  std::shared_ptr<FitsObject> clone(const std::string& filename) const;

private:
  const int id;
  QString  filename;
  std::shared_ptr<FitsImage> image;
  QRect aoi;
  Histogram histogram;
  Profile xprofile;
  Profile yprofile;
  std::unique_ptr<PixelList> pixelList;
  std::unique_ptr<StarList> starList;
  UndoStack undostack;

  static int idCounter;
};

#endif // FILEOBJECT_H
