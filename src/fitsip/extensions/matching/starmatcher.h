/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to match two images based on stars                           *
 *                                                                              *
 * modified: 2025-10-26                                                         *
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

#ifndef STARMATCHER_H
#define STARMATCHER_H

#include "findstars.h"
#include <fitsip/core/fitstypes.h>
#include <fitsip/core/opplugin.h>
#include <QObject>
#include <vector>

class StarMatcherDialog;

class StarMatcher: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  StarMatcher();
  ~StarMatcher();

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> list, const OpPluginData& data=OpPluginData()) override;

  ResultType prepare(FitsImage* image, PixelList* pixellist, bool subsky, int searchbox, int starbox, bool rotate, double maxmove);

  ResultType match(FitsImage* image);

  double getAngle() const;

  double getAngleSigma() const;

  double getDx() const;

  double getSigmadx() const;

  double getDy() const;

  double getSigmady() const;

private:
  std::tuple<double,double> getRotationAngle(const StarList& list1, const StarList& list2);
  std::tuple<double,double,double,double> getShift(const StarList& list1, const StarList& list2);

  StarMatcherDialog* dlg;
  bool subsky;
  int searchbox;
  int starbox;
  double maxmove;
  bool rotate;
  FindStars starfinder;
  StarList starlist1;
  StarList starlist2;
  double angle;
  double angleSigma;
  double dx;
  double sigmadx;
  double dy;
  double sigmady;
};

#endif // STARMATCHER_H
