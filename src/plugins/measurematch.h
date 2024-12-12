/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to match two images                                          *
 *                                                                              *
 * modified: 2022-12-03                                                         *
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

#ifndef MEASUREMATCH_H
#define MEASUREMATCH_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class MeasureMatchDialog;

/**
 * @brief The MeasureMatch plugin matches a region of one image (the templete) to another image.
 *
 * It uses only the gray intensity values of both images. The location of the
 * maximum correlation is printed in the logbook.
 * Algorithm taken from: W.Burger and M.J. Burge, Digitale Bildverarbeitung
 */
class MeasureMatch: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  MeasureMatch();
  virtual ~MeasureMatch() override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> list, QRect aoi=QRect()) override;

  void setTemplate(std::shared_ptr<FitsImage> image, QRect aoi=QRect());

  void computeMatch(std::shared_ptr<FitsImage> image);

  void setMatchFull(bool flag);

  bool isMatchFull() const;

  void setMatchRange(int32_t r);

  int32_t getMatchRange() const;

  void setFirstPassDelta(int32_t d);

  int32_t getFirstPassDelta() const;

  void setSubsample(int32_t s);

  int32_t getSubsample() const;

  void setFactor(int32_t f);

  int32_t getFactor() const;

  double getX() const;

  double getY() const;

  double getDx() const;

  double getDy() const;

  /**
   * @brief Shift the AOI by the given amount.
   *
   * This method can be used to keep the AOI in sync with shifting images
   * allowing for a smaller match range. The shift is always based on the
   * initial AOI set. It is accounted
   * for in the calculated displacement of the matched image.
   * @param dx shift in x
   * @param dy shift in y
   */
  void shiftAOI(double dx, double dy);

private:
  void createI(std::shared_ptr<FitsImage> image);
  double getMatchValue(int32_t r, int32_t s, int32_t n);

  bool matchFull;
  int32_t matchRange;
  int32_t subsample;
  int32_t firstPassDelta;
  int32_t factor;
  std::vector<std::vector<ValueType>> R;
  std::vector<std::vector<ValueType>> I;
  double meanR;
  double sigmaR;
  double max;
  int32_t x;
  int32_t y;
  int32_t dx;
  int32_t dy;
  int32_t offsetX;
  int32_t offsetY;
  int32_t height;
  int32_t width;
  QRect aoi;            // AOI to use
  QRect initialAOI;     // initial AOI
  int32_t aoiShiftX;
  int32_t aoiShiftY;
  MeasureMatchDialog* dlg;

};

#endif // MEASUREMATCH_H
