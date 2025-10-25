/********************************************************************************
 *                                                                              *
 * FitsIP - star data class                                                     *
 *                                                                              *
 * modified: 2025-03-15                                                         *
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

#ifndef STAR_H
#define STAR_H

#include "fitsimage.h"
#include "pixel.h"

/**
 * @brief This class contains all data of a star found by the star finding algorithm.
 */
class Star {
public:
  Star(double x, double y);
  Star(double x, double y, double fwhm, double xw, double yw, double round, double sharp, double hotness);

  double getX() const;

  double getY() const;

  double getFWHM() const;

  double getXWidth() const;

  double getYWidth() const;

  double getRoundness() const;

  double getSharpness() const;

  double getHotness() const;

  void shift(double dx, double dy);

  void rotate(double xc, double yc, double a);

  void rotate(double xc, double yc, double sa, double ca);

  // [[deprecated]]
  // static Star fromPixel(FitsImage* img, ValueType sky, const Pixel& pixel, int box);

private:
  Star();

  // static std::pair<double, double> getCenter(FitsImage* img, ValueType sky, int x0, int y0, int w, int h);
  // static void star_axes(FitsImage* image, int tsx, int tsy, int w, int h, double sky, double *xc, double *yc, double *fwhm, double *xwidth, double *ywidth, int maxiter);
  // static double do_fwhm(FitsImage* image, int sx, int sy, int w, int h, double xc, double yc, double sky);

  double x;         /* x position of center */
  double y;         /* y position of center */
  double fwhm;      /* full width half maximum */
  double xwidth;    /* fwhm width in x direction */
  double ywidth;    /* fwhm width in y direction */
  double round;     /* roundness criterion */
  double sharp;     /* sharpness criterion */
  double hotness;   /* hotness criterion */

};

#endif // STAR_H
