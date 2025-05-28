/********************************************************************************
 *                                                                              *
 * FitsIP - stack images                                                        *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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

#ifndef OPSTACK_H
#define OPSTACK_H

#include <fitsip/extensions/matching/measurematch.h>
#include <fitsip/extensions/matching/findstars.h>
#include <fitsip/core/opplugin.h>
#include <fitsip/core/pixellist.h>
#include <fitsip/core/starlist.h>
#include <QObject>
#include <vector>
#include <tuple>

class OpStackDialog;

class OpStack: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  enum Align { NoAlignment, TemplateMatch, StarMatch };

  OpStack();
  virtual ~OpStack() override;

  virtual bool createsNewImage() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

  virtual ResultType execute(const std::vector<QFileInfo>& list, const OpPluginData& data=OpPluginData()) override;

private:
  ResultType prepare(const QFileInfo& file, bool subsky);
  ResultType prepareTemplate(const QFileInfo& file, bool subsky, QRect aoi, bool full, int range);
  ResultType prepareStarMatch(const QFileInfo& file, PixelList* pixellist, bool subsky, int searchbox, int starbox, bool rotate, double maxmove);
  ResultType stack(const QFileInfo& file);
  ResultType stackTemplate(const QFileInfo& file);
  ResultType stackStarMatch(const QFileInfo& file);
  void findStars(std::shared_ptr<FitsImage> image, StarList* starlist, int searchbox, int starbox);
  Star findStar(std::shared_ptr<FitsImage> image, double sky, int x, int y, int searchbox, int starbox);
  std::tuple<double,double> getRotationAngle(const StarList& list1, const StarList& list2);
  std::tuple<double,double,double,double> getShift(const StarList& list1, const StarList& list2);

  OpStackDialog* dlg;
  std::shared_ptr<FitsImage> img;
  bool subtractSky;
  MeasureMatch matcher;
  int searchbox;
  int starbox;
  double maxmove;
  bool rotate;
  FindStars starfinder;
  StarList starlist1;
  StarList starlist2;
};

#endif // OPSTACK_H
