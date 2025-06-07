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

#include "star.h"
#include "math/gaussfit.h"
#include "math/utils.h"

Star::Star():
  x(-1),
  y(-1),
  fwhm(0),
  xwidth(0),
  ywidth(0),
  round(0),
  sharp(0),
  hotness(0)
{
}

Star::Star(double x, double y):
  x(x),
  y(y),
  fwhm(0),
  xwidth(0),
  ywidth(0),
  round(0),
  sharp(0),
  hotness(0)
{
}

Star::Star(double x, double y, double fwhm, double xw, double yw, double round, double sharp, double hotness):
  x(x),
  y(y),
  fwhm(fwhm),
  xwidth(xw),
  ywidth(yw),
  round(round),
  sharp(sharp),
  hotness(hotness)
{
}

double Star::getX() const
{
  return x;
}

double Star::getY() const
{
  return y;
}

double Star::getFWHM() const
{
  return fwhm;
}

double Star::getXWidth() const
{
  return xwidth;
}

double Star::getYWidth() const
{
  return ywidth;
}

double Star::getRoundness() const
{
  return round;
}

double Star::getSharpness() const
{
  return sharp;
}

double Star::getHotness() const
{
  return hotness;
}

void Star::shift(double dx, double dy)
{
  x += dx;
  y += dy;
}

void Star::rotate(double xc, double yc, double a)
{
  a *= M_PI / 180.0;
  double ca = cos(a);
  double sa = sin(a);
  double xr = (x - xc) * ca - (y - yc) * sa;
  double yr = (x - xc) * sa + (y - yc) * ca;
  x = xr + xc;
  y = yr + yc;
}

void Star::rotate(double xc, double yc, double sa, double ca)
{
  double xr = (x - xc) * ca - (y - yc) * sa;
  double yr = (x - xc) * sa + (y - yc) * ca;
  x = xr + xc;
  y = yr + yc;
}




Star Star::fromPixel(FitsImage* img, ValueType sky, const Pixel& pixel, int box)
{
  QRect sr(pixel.x-box/2,pixel.y-box/2,box,box);
  sr = img->getOverlap(sr);
  int maxiter = 20;
  double xc;
  double yc;
  double fwhm;
  double xwidth;
  double ywidth;
  star_axes(img,sr.x(),sr.y(),sr.width(),sr.height(),sky,&xc,&yc,&fwhm,&xwidth,&ywidth,maxiter);
  Star star(xc,yc,fwhm,xwidth,ywidth,0,0,0);
  return star;
}



/* compute the centroid */
std::pair<double, double> Star::getCenter(FitsImage* img, ValueType sky, int x0, int y0, int w, int h)
{
  double sum = 0.0;
  double sumx = 0.0;
  double sumy = 0.0;
  for (int j=0;j<h;++j)
  {
    auto it = img->getConstPixelIterator(x0,y0);
    for (int i=0;i<w;++i)
    {
      double val = it.getAbs() - sky;
      if (val > 0)
      {
        sum += val;
        sumx += val * (x0+i);
        sumy += val * (y0+j);
      }
      ++it;
    }
  }
  /*
   * make a sanity check
   */
  if (sum <= 0)
  {
    return std::make_pair(-1.0,-1.0);
  }
  double xc = sumx / sum;		/* centroid in x */
  double yc =  sumy / sum;		/* centroid in y */
  return std::make_pair(xc,yc);
}



/********************************************************************************
 * Algorithm adapted from PCVISTA. Respective copyright notice follows:         *
 ********************************************************************************
 *                                                                              *
 * Copyright (c) 1989,1990,1991 Michael Richmond, Richard Treffers and          *
 *                    The Regents of the University of California               *
 *                                                                              *
 *                    This software may be copied and distributed for           *
 *                    educational, research and not for profit services         *
 *                    provided that this copyright and statement are            *
 *                    included in all such copies.                              *
 *                                                                              *
 ********************************************************************************
 * Algorithm adapted from XVISTA. Respective copyright notice follows:          *
 ********************************************************************************
 *                                                                              *
 * Copyright (c) 1996 Michael Richmond and Richard Treffers                     *
 *                                                                              *
 *                    This software may be copied and distributed for           *
 *                    educational, research and not for profit services         *
 *                    provided that this copyright and statement are            *
 *                    included in all such copies.                              *
 *                                                                              *
 ********************************************************************************/

void Star::star_axes(FitsImage* image, int tsx, int tsy, int w, int h, double sky, double *xc, double *yc, double *fwhm, double *xwidth, double *ywidth, int maxiter)
{
//  static int i, row, col, er, ec, bigger, fw_flag;
//  static int16 val;
//  static float chisq, amp, center, width;
#ifdef NORMALIZE
  static float minval;
#endif
//  static double pix, dxx;
#ifdef NEW
  static int fitrad;
  static double peak, eccen, major_axis, minor_axis;
#endif

  /* note that THERE IS SKY in the image supplied to this routine. */

  int fw_flag = 0;
  int ex = tsx + w;
  int ey = tsy + h;

#ifdef DEBUG
  printf("into star_axes: tsx=%d tsy=%d w=%d h=%d\n", tsx, tsy, w, h);
#endif

#ifdef NEW
  fitrad = (nr > nc ? nc/2 : nr/2);
  if (do_axes(image, tsr, tsc, nr, nc, sky, fitrad,
              xc, yc, xwidth, ywidth,
              fwhm, &peak, &eccen,
              &major_axis, &minor_axis, maxiter) != 0) {
    *xc = tsr;
    *yc = tsc;
    *fwhm = -1.0;
    *xwidth = -1.0;
    *ywidth = -1.0;
    return;
  }
  return;
#endif

  double sum = 0.0;
  double sumx = 0.0;
  double sumy = 0.0;
  /* compute the centroid */
  for (int y=0;y<h;y++)
  {
    ConstPixelIterator it = image->getConstPixelIterator(tsx,tsy+y);
    for (int x=0;x<w;x++)
    {
      double val = it.getAbs() - sky;
      if (val > 0)
      {
        sum += val;
        sumx += val * x;
        sumy += val * y;
      }
      ++it;
    }
  }
  /*
   * make a sanity check: if the "lsum" is <= 0, then
   * we have some seriously screwed-up stellar candidate.
   * In such a case, immediately return
   * with bogus values (-1.0) in fwhm, xwidth, ywidth.  That should
   * ensure that this "star" is classified as some kind of noise.
   */
  if (sum <= 0)
  {
#ifdef DEBUG
    printf("lsum <= 0: xc = %lf yc = %lf \n", *xc, *yc);
#endif
    *xc = tsx;
    *yc = tsy;
    *fwhm = -1.0;
    *xwidth = -1.0;
    *ywidth = -1.0;
    return;
  }
  double xc_ax = sumx / sum;		/* centroid in x */
  double yc_ax =  sumy / sum;		/* centroid in y */


  /* upcoming section, which recalculates the center of light, added 10/4/92
     by MWR. DO NOT remove the section above - we need it for calculating
     the second moments later on */

  /* allocate the vectors we'll need */
  int bigger = std::max(w,h);
  ValueType* x = new ValueType[bigger];
  ValueType* y = new ValueType[bigger];
  ValueType* sig = new ValueType[bigger];
  for (int i=0;i<bigger;i++) sig[i] = 1.0;

  /* first do along the Y direction */
  for (int i=0;i<h;i++)
  {
    x[i] = i + tsy;
    y[i] = 0;
    ConstPixelIterator it = image->getConstPixelIterator(tsx,tsy+i);
    for (int j=0;j<w;j++)
    {
      y[i] += it.getAbs() - sky;
      ++it;
    }
  }

#ifdef NORMALIZE
  /* here I "normalize" the marginal sums so that the lowest one is set
     to zero.  the gaussian fitting routine assumes the baseline is zero */
  minval = y[0];
  for (int i=0;i<nr;i++)
  {
    if (y[i] < minval) minval = y[i];
  }
  for (int i=0;i<nr;i++) y[i] -= minval;
#endif

  //  if (gauss_fit(x, y, sig, nr, &amp, &center, &width, &chisq, PIXEL_CENTER,
  //                 maxiter) == 0) {
  //    xc_ax = center;
  //    *xwidth = 2.35*width;	/* convert to FWHM */
  //  }
  GaussFit gauss;
  gauss.setMaxIteration(maxiter);
  if (gauss.fit(x,y,sig,h) == 0)
  {
    yc_ax = gauss.getCenter();
    *ywidth = 2.35 * gauss.getSigma();  /* convert to FWHM */
  }
  else
  {
#ifdef DEBUG
    printf("star_axes: gauss_fit returns non-zero in Y direction\n");
#endif
    *ywidth = -1;	/* a sign to use 2nd moment below */
  }

  /* now do along the x direction */
  for (int i=0;i<w;i++)
  {
    x[i] = i + tsx;
    y[i] = 0;
    ConstPixelIterator it = image->getConstPixelIterator(tsx+i,tsy);
    for (int j=0;j<h;j++)
    {
      y[i] += it.getAbs() - sky;
      it += image->getWidth();
    }
  }

#ifdef NORMALIZE
  /* here I "normalize" the marginal sums so that the lowest one is set
     to zero.  the gaussian fitting routine assumes the baseline is zero */
  minval = y[1];
  for (int i=0;i<nc;i++)
  {
    if (y[i] < minval) minval = y[i];
  }
  for (int i=0;i<nr;i++) y[i] -= minval;
#endif

  //  if (gauss_fit(x, y, sig, nr, &amp, &center, &width, &chisq, PIXEL_CENTER,
  //                maxiter) == 0) {
  //    yc_ax = center;
  //    *ywidth = 2.35*width;	/* convert to FWHM */
  //  }
  if (gauss.fit(x,y,sig,w) == 0)
  {
    xc_ax = gauss.getCenter();
    *xwidth = 2.35 * gauss.getSigma();  /* convert to FWHM */
  }
  else
  {
#ifdef DEBUG
    printf("star_axes: gauss_fit returns non-zero in X direction\n");
#endif
    *xwidth = -1;
  }
  delete [] x;
  delete [] y;
  delete [] sig;

  *xc = xc_ax;
  *yc = yc_ax;
#ifdef DEBUG
  printf("xc_ax = %lf xc = %lf  yc_ax=%lf yc = %lf\n", xc_ax, yc_ax, *xc, *yc);
#endif

  /* now make a sanity check: if the centroid falls outside the
     small box (tsr, er, tsc, ec), then immediately return
     with bogus values (-1.0) in fwhm, xwidth, ywidth.  That should
     assure this "star" of being classified as some kind of noise */
  if ((*xc < tsx) || (*xc > ex) || (*yc < tsy) || (*yc > ey))
  {
#ifdef DEBUG
    printf("centroid out of smallbox bounds: xc = %lf yc = %lf \n", *xc, *yc);
#endif
    *xc = tsx;
    *yc = tsy;
    *fwhm = -1.0;
    *xwidth = -1.0;
    *ywidth = -1.0;
    return;
  }

  double sumxx = 0;
  double sumyy = 0;
  /* now calculate the second moments to get a measure for the
     'width' of the image (in case the gaussian-fit failed) */
  for (int row=tsy;row<ey;row++)
  {
    double dyy = (row - yc_ax) * (row - yc_ax);
    ConstPixelIterator it = image->getConstPixelIterator(tsx,row);
    for (int col=tsx;col<ex;col++)
    {
      double val = it.getAbs() - sky;
      if (val > 0)
      {
        sumyy += dyy*val;
        sumxx += (col - xc_ax)*(col - xc_ax)*val;
      }
    }
  }
  if (*xwidth == -1.0)
  {
    fw_flag = 1;
    *xwidth = sqrt(sumxx/sum);
  }
  if (*ywidth == -1.0)
  {
    fw_flag = 1;
    *ywidth = sqrt(sumyy/sum);
  }
  if (fw_flag == 1)
    *fwhm = do_fwhm(image,tsx,tsy,w,h,xc_ax,yc_ax,sky);
  else
    *fwhm = 0.5 * (*xwidth + *ywidth);
}

double Star::do_fwhm(FitsImage* image, int sx, int sy, int w, int h, double xc, double yc, double sky)
{
//  int i, j, cent_col, cent_row, halfmax;
//	double frac, d1, d2, d3, d4;

#ifdef DEBUG
  printf("into do_fwhm, sx=%d sy=%d xc=%lf yc=%lf  here comes the data\n", sx, sy, xc, yc);
#endif

  int cent_y = (int) (yc + 0.5);		/* approximations to center */
  int cent_x = (int) (xc + 0.5);		/* in box coords */
  /* check to make sure center pixel in bounds */
  if (cent_x >= sx + w) cent_x = (sx + w) - 1;
  if (cent_y >= sy + h) cent_y = (sy + h) - 1;
  double halfmax = (image->getConstPixelIterator(cent_x,cent_y).getAbs() - sky) / 2 + sky;

  int i;
  ConstPixelIterator it = image->getConstPixelIterator(cent_x,cent_y);
  for (i=cent_x;i>sx;i--)
  {
    if (it.getAbs() < halfmax)	break;
    --it;
  }
  double d1;
  if (i + 1 >= sx + w)
  {
    d1 = 0.0;
  }
  else
  {
    double v1 = it.getAbs();
    ++it;
    double v2 = it.getAbs();
    double frac = math::get_frac(v2,v1,halfmax);
    d1 = (cent_x - (i+1)) + frac;
  }
  it = image->getConstPixelIterator(cent_x,cent_y);
  for (i=cent_x;i<(sx+w)-1;i++)
  {
    if (it.getAbs() < halfmax) break;
    ++it;
  }
  double d2;
  if (i < sx)
  {
    d2 = 0.0;
  }
  else
  {
    double v1 = it.getAbs();
    --it;
    double v2 = it.getAbs();
    double frac = math::get_frac(v2,v1,halfmax);
    d2 = ((i - 1) - cent_x) + frac;
  }
  it = image->getConstPixelIterator(cent_x,cent_y);
  int j;
  for (j=cent_y;j>h;j--)
  {
    if (it.getAbs() < halfmax) break;
    it -= image->getWidth();
  }
  double d3;
  if (j + 1 >= sy + h)
  {
    d3 = 0.0;
  }
  else
  {
    double v1 = it.getAbs();
    it += image->getWidth();
    double v2 = it.getAbs();
    double frac = math::get_frac(v2,v1,halfmax);
    d3 = (cent_y - (j+1)) + frac;
  }
  it = image->getConstPixelIterator(cent_x,cent_y);
  for (j = cent_y; j < (sy + h) - 1; j++)
  {
    if (it.getAbs() < halfmax) break;
    it += image->getWidth();
  }
  double d4;
  if (j - 1 < sy)
  {
    d4 = 0.0;
  }
  else
  {
    double v1 = it.getAbs();
    it -= image->getWidth();
    double v2 = it.getAbs();
    double frac = math::get_frac(v2,v1,halfmax);
    d4 = ((j - 1) - cent_y) + frac;
  }
  return ((d1 + d2) + (d3 + d4)) / 2.0;
}





