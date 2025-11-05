/********************************************************************************
 *                                                                              *
 * FitsIP - plugin to match two images                                          *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#include <fitsip/core/fitstypes.h>
#include <fitsip/core/opplugin.h>
#include <QObject>
#include <vector>

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
  Q_INTERFACES(OpPlugin)
public:
  MeasureMatch();
  virtual ~MeasureMatch() override;

  virtual QString getMenuEntry() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> list, const OpPluginData& data=OpPluginData()) override;

  void setTemplate(const FitsImage& image, QRect aoi=QRect());

  void computeMatch(const FitsImage& image);

  void setMatchFull(bool flag);

  bool isMatchFull() const;

  void setMatchRange(int r);

  int getMatchRange() const;

  void setFirstPassDelta(int d);

  int getFirstPassDelta() const;

  void setSubsample(int s);

  int getSubsample() const;

  void setFactor(int f);

  int getFactor() const;

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
  void createI(const FitsImage& image);
  double getMatchValue(int r, int s, int n);

  bool matchFull;
  int matchRange;
  int subsample;
  int firstPassDelta;
  int factor;
  std::vector<std::vector<ValueType>> R;
  std::vector<std::vector<ValueType>> I;
  double meanR;
  double sigmaR;
  double max;
  int x;
  int y;
  int dx;
  int dy;
  int offsetX;
  int offsetY;
  int height;
  int width;
  QRect aoi;            // AOI to use
  QRect initialAOI;     // initial AOI
  int aoiShiftX;
  int aoiShiftY;
  MeasureMatchDialog* dlg;

};

#endif // MEASUREMATCH_H
