/********************************************************************************
 *                                                                              *
 * FitsIP - star detection class                                                *
 *                                                                              *
 * modified: 2025-08-27                                                         *
 ********************************************************************************
 * Copyright (C) by Harald Braeuning.                                           *
 ********************************************************************************
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

#include "findstars.h"
#include "findstarsdialog.h"
#include <fitsip/coreplugins/optogray.h>
#include <fitsip/core/fitsimage.h>
#include <fitsip/core/histogram.h>
#include <fitsip/core/starlist.h>
#include <fitsip/core/math/gaussfit.h>
#include <cmath>
#include <algorithm>
#include <QDebug>

#define DEBUG

FindStars::FindStars():OpPlugin(),
  c_biton(0),
  c_abovesig(0),
  c_peak(0),
  c_1validfwhm(0),
  c_2validfwhm(0),
  c_movedaway(0),
  c_round(0),
  c_sharp(0),
  c_hot(0),
  c_star(0),
  skysig(0),
  minsig(3.0),
  starfwhm(5.0),        /* default fwhm of stellar features */
  minfwhm(2.0),         /* min allowed stellar profile fwhm */
  maxfwhm(12.0),         /* max ditto */
  minround(-1.0),        /* min allowed 'round' criterion for a valid star */
  maxround(1.0),        /* max ditto */
  minsharp(0.3),        /* min allowed 'sharp' criterion for a valid star */
  maxsharp(0.9),        /* max ditto */
  cminsharp(0.2),       /* min 'sharp' criterion, when convolving */
  cmaxsharp(1.0),       /* max ditto */
  minhot(-1.0E6),          /* min allowed 'hotness' criterion for a valid star */
  maxhot(1.0E6),          /* max ditto */
  movemax(1.0),
  sky(0),
  quickflag(true),
  peakrad(1.0*starfwhm),
  tinysize(9),
  maxiter(20)
{
  profiler = SimpleProfiler("FindStars");
}

FindStars::~FindStars()
{
}

std::vector<std::shared_ptr<FitsObject>> FindStars::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(conv_img)};
}

QString FindStars::getMenuEntry() const
{
  return "Analyse/Find Stars...";
}

OpPlugin::ResultType FindStars::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  Histogram hist;
  hist.build(image->getImage().get());
  AverageResult avg = hist.getAverage(0.75);
  FindStarsDialog d;
  d.setSkyMean(avg.mean);
  d.setSkySigma(avg.sigma);
  d.setSkyMinSigma(minsig);
  d.setFWHMMin(minfwhm);
  d.setFWHMMax(maxfwhm);
  d.setRoundnessMin(minround);
  d.setRoundnessMax(maxround);
  d.setSharpnessMin(minsharp);
  d.setSharpnessMax(maxsharp);
  d.setHotnessMin(minhot);
  d.setHotnessMax(maxhot);
  d.setTinyboxSize(tinysize);
  d.setMoveMax(movemax);
  d.setBlur(!quickflag);
  d.setIterations(maxiter);
  if (d.exec())
  {
    sky = d.getSkyMean();
    skysig = d.getSkySigma();
    minsig = d.getSkyMinSigma();
    minfwhm = d.getFWHMMin();
    maxfwhm = d.getFWHMMax();
    minround = d.getRoundnessMin();
    maxround = d.getRoundnessMax();
    minsharp = d.getSharpnessMin();
    maxsharp = d.getSharpnessMax();
    minhot = d.getHotnessMin();
    maxhot = d.getHotnessMax();
    tinysize = d.getTinyboxSize();
    movemax = d.getMoveMax();
    quickflag = !d.isBlur();
    maxiter = d.getIterations();
    profiler.start();
    auto img = image->getImage();
    if (!data.aoi.isEmpty())
    {
      img = img->subImage(data.aoi);
    }
//    if (image->getDepth() > 1)
//    {
      image = OpToGray().toGray(image);
//    }
    c_biton = 0;
    c_abovesig = 0;
    c_peak = 0;
    c_1validfwhm = 0;
    c_2validfwhm = 0;
    c_movedaway = 0;
    c_round = 0;
    c_sharp = 0;
    c_hot = 0;
    c_star = 0;
    std::vector<Star> stars = findStars(img);
    data.starlist->setStars(stars);
    data.starlist->shift(data.aoi.x(),data.aoi.y());
    profiler.stop();
    logProfiler(image->getName());
    qDebug() << "FindStars:";
    qDebug() << "  c_abovesig" << c_abovesig;
    qDebug() << "  c_peak" << c_peak;
    qDebug() << "  c_1validfwhm" << c_1validfwhm;
    qDebug() << "  c_biton" << c_biton;
    qDebug() << "  c_2validfwhm" << c_2validfwhm;
    qDebug() << "  c_movedaway" << c_movedaway;
    qDebug() << "  c_round" << c_round;
    qDebug() << "  c_sharp" << c_sharp;
    qDebug() << "  c_hot" << c_hot;
    qDebug() << "  c_star" << c_star;
    return OK;
  }
  return CANCELLED;
}

std::vector<Star> FindStars::findStars(std::shared_ptr<FitsImage> image)
{
  std::vector<Star> stars;
  int htiny = tinysize;
  int wtiny = tinysize;
  int num_star = 0;

  /* first, create and initialize to zero the bit map */
  bitmap = std::vector<uint8_t>(image->getWidth()*image->getHeight(),0);
  /* if (quickflag == false), convolve the image with a gaussian and put
     the convolved version into the 'conv_image[]' array.
     the stdev of counts in the convolved image is returned as 'conv_sig' */
  double signif;
  if (quickflag)
  {
    conv_img = image;
    signif = sky + (minsig * skysig);
  }
  else
  {
    /*
     * we use "skysig" as a first estimate of the expected
     * standard deviation of values in the convolved image;
     * it allows us to calculate a clipped mean very quickly.
     */
//    double conv_sig = skysig;
//    signif = sky + minsig * skysig;
    conv_img = convolve(image,starfwhm);
    double conv_sig = find_skysig(conv_img,skysig);
    signif = minsig * conv_sig;
    sky = 0;
    minsharp = cminsharp;
    maxsharp = cmaxsharp;
  }
  qDebug() << "signif" << signif;
  /* next, go through the image looking for peaks which haven't been marked yet */
  ConstPixelIterator it = conv_img->getConstPixelIterator();
  for (int y=0;y<conv_img->getHeight();++y)
  {
    for (int x=0;x<conv_img->getWidth();++x,++it)
    {
//      qDebug() << x << "," << y << ": " << it.getAbs();
      if (it.getAbs() < signif)
      {
        continue;
      }
      c_abovesig++;
      /* first, let's see if this data value is a peak */
      if (!is_peak(conv_img,x,y))
      {
        continue;
      }
      c_peak++;

      int tsx,tsy;
      calc_box(x,y,conv_img->getWidth(),conv_img->getHeight(),&tsx,&tsy,wtiny,htiny);

      double xc,yc,fwhm,xwidth,ywidth;
      star_axes(conv_img,tsx,tsy,wtiny,htiny,sky,&xc,&yc,&fwhm,&xwidth,&ywidth,maxiter);

      if ((fabs(xc-x) > movemax) || (fabs(yc-y) > movemax))
      {
        qDebug() << "candidate" << x << "," << y << "has moved outside limits: " << std::max(fabs(xc-x),fabs(yc-y));
        continue;
      }

      /* depending on the results, call it a star or don't */
      if (!validFWHM(fwhm))
      {
        qDebug() << "candidate" << x << "," << y << "has FWHM outside limits:" << fwhm;
        continue;
      }
      c_1validfwhm++;

      /* there used to be a loop here, but I deleted it - it slowed
         things down and threw out some good stars.  MWR */
      /* added checks on 'irow' and 'icol' to make sure they're
         inside the image.  MWR 12/13/92 */
      int iy = (int)(yc + 0.5);
      if (iy >= conv_img->getHeight()) iy = conv_img->getHeight() - 1;
      int ix = (int)(xc + 0.5);
      if (ix >= conv_img->getWidth()) ix = conv_img->getWidth() - 1;

      if (bitmap[iy*image->getWidth()+ix])
      {
        c_biton++;
        continue;
      }
      if (!validFWHM(fwhm))
      {
        c_2validfwhm++;
        continue;
      }

      double round = 2.0 * ((xwidth - ywidth) / (xwidth + ywidth));
      if ((round < minround) || (round > maxround))
      {
        qDebug() << "candidate" << x << "," << y << "has roundness outside limits:" << round;
        c_round++;
        continue;
      }

      double sharp = sharpness(conv_img,ix,iy);
      if (sharp == NAN || (sharp < minsharp) || (sharp > maxsharp))
      {
        qDebug() << "candidate" << x << "," << y << "has sharpness outside limits:" << sharp;
        c_sharp++;
        continue;
      }

      /* check to see if the peak pixel looks like a hot pixel/CR */
      double hot_factor = hotness(conv_img,x,y);
      if ((hot_factor < minhot) || (hot_factor > maxhot))
      {
        qDebug() << "candidate" << x << "," << y << "has hotness outside limits:" << hot_factor;
        c_hot++;
        continue;
      }

//      if (showflag) {
//#ifdef XWINDOWS
//        do_crosshair(irow, icol);
//#endif
//      }

//      /*
//       * should we print out the FWHM for each detected object,
//       *   or a width based on the second moments?   The default is
//       *   to print FWHM, but the user can request the second-moment-
//       *   based value via a command-line option "secondmom".
//       */
//      if (moment_flag == 0) {
//        width_value = fwhm;
//      }
//      else {
//        width_value = 0.5*(xwidth + ywidth);
//      }
      printf("%5d %7.2f   %7.2f   %7.2f   %6.3f    %6.3f   %6.3f \n",
          ++num_star, xc, yc,	conv_img->getConstPixelIterator(ix,iy).getAbs() - sky,
          fwhm, round, sharp);
      Star star(xc,yc,fwhm,xwidth,ywidth,round,sharp,hot_factor);
      stars.push_back(star);
      c_star++;

      /* if it WAS a star, mark out all the pixels around it */
      markpix(image,xc,yc,fwhm);
//       if(num_star == maxstars)
//      error(1,"stars: too many stars found");
    }
  }
  return stars;
}

void FindStars::starAxes(std::shared_ptr<FitsImage> image, const QRect &box, double sky, double *xc, double *yc, double *fwhm, double *xwidth, double *ywidth, int maxiter)
{
  star_axes(image,box.x(),box.y(),box.width(),box.height(),sky,xc,yc,fwhm,xwidth,ywidth,maxiter);
}






std::shared_ptr<FitsImage> FindStars::convolve(std::shared_ptr<FitsImage> image, double fwhm)
{
  Gaussian gauss(fwhm);
  auto conv_img = std::make_shared<FitsImage>("tmp",image->getWidth(),image->getHeight(),1);
  int offset = gauss.n / 2;
  /* now, write real numbers to all of the image that we can */
  for (int y=offset;y<image->getHeight()-offset;y++)
  {
    PixelIterator it = conv_img->getPixelIterator(offset,y);
    for (int x=offset;x<image->getWidth()-offset;x++)
    {
      double v = do_gauss(image,x,y,gauss);
      /*
       * tests on TASS images show that the negative
       * "moats" around star centers in the convolved
       * image can cause the centroiding routine to go
       * haywire.  Let's replace negative values with zero
       * in the convolved image, to avoid this.
       *     MWR 3/16/1997
       */
#ifdef CLIPNEGATIVE
      if (v < 0) v = 0;
#endif
      it[0] = v;
      ++it;
    }
  }
  return conv_img;
}

double FindStars::do_gauss(std::shared_ptr<FitsImage> image, uint32_t x, uint32_t y, const Gaussian& gauss)
{
  uint32_t sx = x - gauss.n / 2;
  uint32_t sy = y - gauss.n / 2;
  double sum = 0;
  double dsum = 0;
  const double* dq = gauss.data.data();
  for (uint32_t i=0;i<gauss.n;i++)
  {
    ConstPixelIterator it = image->getConstPixelIterator(sx,sy+i);
    for (uint32_t j=0;j<gauss.n;j++)
    {
      double v = it.getAbs();
      sum += v * (*dq++);
      dsum += v;
      ++it;
    }
  }
  sum = (sum - dsum * gauss.gsum) / gauss.gnum;		/* this is the "extra" bit */
  return sum;
}

#define CLIPSIG    3.0        /* clip values more than this many stdev */
                              /* from 0.0 when calculating sky-sig in  */
                              /* sg_skysig routine. */

/* calculate the standard deviation of pixels from the mean in the
   convolved image.  We are a little tricky here:

  - we know the mean of the convolved image will be very
       close to 0.0
  - we can use the "rough_sig" argument (which is based
       on the standard deviation of sky in the UN-convolved image)
       as a guess at the width of the sky in the convolved image

   So, what we do is calculate a CLIPPED mean and standard deviation,
   using only pixels which have values between

  -CLIPSIG*rough_sig < value < CLIPSIG*rough_sig

   and we ignore all others.

   Return a value of 1.0 if we can't calculate the standard
   deviation formally.
*/
double FindStars::find_skysig(std::shared_ptr<FitsImage> image, double rough_sig)
{
  double total = 0.0;
  int pixnum = 0;
  double sumsq = 0.0;
  double minval = -CLIPSIG * rough_sig;
  double maxval = CLIPSIG * rough_sig;
  ConstPixelIterator it = image->getConstPixelIterator();
  for (int i=0;i<image->getWidth()*image->getHeight();i++)
  {
    double number = it.getAbs();
    if ((number >= minval) && (number <= maxval))
    {
      total += (double)number;
      sumsq += (double)number*(double)number;
      pixnum++;
    }
  }
  if (pixnum < 2) return 1.0;
  double mean = total / pixnum;
  double diff = (sumsq - pixnum*mean*mean);
  double stdev = 1.0;
  if (diff > 0.0)
  {
    stdev = sqrt(diff/(pixnum-1));
  }
  return stdev;
}

bool FindStars::is_peak(std::shared_ptr<FitsImage> image, uint32_t xc, uint32_t yc)
{
  if (xc < peakrad) return 0;
  uint32_t sx = xc - peakrad;
  if (yc < peakrad) return 0;
  uint32_t sy = yc - peakrad;
  uint32_t ex = xc + peakrad;
  if (ex >= image->getWidth()) return 0;
  uint32_t ey = yc + peakrad;
  if (ey >= image->getHeight()) return 0;

  double val = image->getConstPixelIterator(xc,yc).getAbs();
  for (uint32_t y=sy;y<ey;y++)
  {
    ConstPixelIterator it = image->getConstPixelIterator(sx,y);
    for (uint32_t x=sx;x<ex;x++)
    {
      if (val < it.getAbs())	return false;
      ++it;
    }
  }
  return true;
}

void FindStars::calc_box(int x, int y, int w, int h, int *tsx, int *tsy, int wsize2, int hsize2)
{
  if (x < (wsize2 / 2))
    *tsx = 0;
  else if ((x + (wsize2/2) + 1) > w)
    *tsx = w - wsize2;
  else
    *tsx = x - wsize2 / 2;
  if (y < (hsize2 / 2))
    *tsy = 0;
  else if ((y + (hsize2/2) + 1) > h)
    *tsy = h - hsize2;
  else
    *tsy = y - hsize2 / 2;
}

void FindStars::star_axes(std::shared_ptr<FitsImage> image, int tsx, int tsy, int w, int h, double sky, double *xc, double *yc, double *fwhm, double *xwidth, double *ywidth, int maxiter)
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

double FindStars::do_fwhm(std::shared_ptr<FitsImage> image, int sx, int sy, int w, int h, double xc, double yc, double sky)
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
    double frac = get_frac(v2,v1,halfmax);
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
    double frac = get_frac(v2,v1,halfmax);
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
    double frac = get_frac(v2,v1,halfmax);
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
    double frac = get_frac(v2,v1,halfmax);
    d4 = ((j - 1) - cent_y) + frac;
  }
  return ((d1 + d2) + (d3 + d4)) / 2.0;
}

/* given two numbers 'larger' and 'smaller', return the fraction of
   the way from 'larger' to 'smaller' that you have to go to get
   to the number 'middle'.

   i.e.  larger=10, smaller=4, middle=6.0 --> return 0.66

 */
double FindStars::get_frac(double larger, double smaller, double middle)
{
  double a = larger - smaller;
  double b = larger - middle;
  if (a <= b || a <= 0.0)	return(0.0);
  return b / a;
}

bool FindStars::validFWHM(double fwhm)
{
  if (fwhm < minfwhm) return false;
  if (fwhm > maxfwhm) return false;
  return true;
}

/*
  mark all the pixels within a radius of peakrad around a star centroid
  as "taken", so that if another star is found here, we won't report it.

  this routine used to mark all stars within one FWHM as taken, but I think
  that's not as good as this - in some cases, two very close stars would
  be missed by that approach.

  'xc' is the row coord of the star
  'yc' is the col coord ditto
*/
void FindStars::markpix(std::shared_ptr<FitsImage> image, double xc, double yc, double fwhm)
{
  /* here is the new version */
  int msx = xc < peakrad ? 0 : static_cast<int>(xc-peakrad);
  int msy = yc < peakrad ? 0 : static_cast<int>(yc-peakrad);
  int mex = static_cast<int>(xc+peakrad);
  if (mex < image->getWidth()-1)	mex = image->getWidth() - 1;
  int mey = static_cast<int>(yc+peakrad);
  if (mey < image->getHeight()-1)	mey = image->getHeight() - 1;
  for (int i=msy;i<=mey;i++)
  {
    for (int j=msx;j<=mex;j++)
    {
      bitmap[i*image->getWidth()+j] = 1;
    }
  }
}

/*******************************************************************
 * PROCEDURE: hotness
 *
 * DESCRIPTION: Given the location of a significant peak pixel,
 *              which might mark a star, check to see if the
 *              peak is likely to be an artifact: a hot pixel
 *              or a cosmic ray.
 *
 *              We use very simple, very fast check.  Will catch
 *              only single-pixel events.
 *
 *              The "hotness" of a peak depends on the values of the
 *              pixel in question and four closest neighbors,
 *              from all of which we subtract the sky estimate first.
 *              We then compute a sum with weights like so:
 *
 *                          0     -1     0
 *                         -1      1    -1
 *                          0     -1     0
 *
 *              A single high pixel value, surrounded by pixels with
 *              little or no signal, will have a large sum;
 *              the peak of a real star will have a small sum,
 *              as long as the FWHM is larger than 1 pixel.
 *
 *              Note: this is just first pass, will likely compute
 *              a ratio later.  MWR 4/13/2017
 *
 * RETURNS:     the "hotness" index of the star.
 */
double FindStars::hotness(std::shared_ptr<FitsImage> image, uint32_t x, uint32_t y)
{
  double sum = 0;
  /* subtract the contributions from the neighbors */
  if (x >= 1)
  {
    sum -= image->getConstPixelIterator(x-1,y).getAbs() - sky;
  }
  if (y >= 1)
  {
    sum -= image->getConstPixelIterator(x,y-1).getAbs() - sky;
  }
  if ((x + 1) < image->getWidth())
  {
    sum -= image->getConstPixelIterator(x+1,y).getAbs() - sky;
  }
  if ((y + 1) < image->getHeight())
  {
    sum -= image->getConstPixelIterator(x,y+1).getAbs() - sky;
  }
  /* and now the contribution from the peak pixel itself */
  sum += image->getConstPixelIterator(x,y).getAbs() - sky;
  return sum;
}

/* return the 'SHARPNESS' index of the star, given by

(image value at star centroid) - (mean of image values around centroid)
-----------------------------------------------------------------------
                (value of convolved image H at star centroid)

   if can't find any values around the centroid, return BADVAL
   (which is very large).

6/3/92 - edge bug fix -rrt
*******/
double FindStars::sharpness(std::shared_ptr<FitsImage> image, uint32_t x, uint32_t y)
{
  double sum = 0;
  uint32_t ssx = x < peakrad ? 0 : x - peakrad;
  uint32_t ssy = y < peakrad ? 0 : y - peakrad;
  uint32_t sex = x + peakrad;
  if (sex > image->getWidth()) sex = image->getWidth();
  uint32_t sey = y + peakrad;
  if (sey > image->getHeight()) sey = image->getHeight();

  int num = 0;
  for (uint32_t i = ssy;i<sey;i++)
  {
    ConstPixelIterator it = image->getConstPixelIterator(ssx,i);
    for (uint32_t j=ssx;j<sey;j++)
    {
      sum += it.getAbs() - sky;
      ++it;
      num++;
    }
  }
  double val = image->getConstPixelIterator(x,y).getAbs() - sky;
  sum -= val;
  num--;
  if (num > 0) sum /= num;
  if (val > 0)
    return (val - sum) / val;
  else
    return NAN;
}


FindStars::Gaussian::Gaussian(double fwhm)
{
  double sigma = fwhm / 2.355;
  /* allocate space for the array */
  n = static_cast<uint32_t>(fwhm * 2.0 + 0.5);
  if ((n % 2) == 0) n++;
  data = std::vector<double>(n*n);
  /* and now fill the array with appropriate numbers */
  double mid = (n - 1) / 2;
  double ss = 2.0 * sigma * sigma;
  gsum = 0;
  gsum2 = 0;
  double *d = data.data();
  for (uint32_t i=0;i<n;i++)
  {
    double y = (mid - i);
    for (uint32_t j=0;j<n;j++)
    {
      double x = (mid - j);
      double z = (x*x + y*y) / ss;
      *d = exp(-z);
      gsum += *d;
      gsum2 += *d * *d;
      ++d;
    }
  }
  /* and now, calculate quantities we'll need later */
  gnum = (gsum2 - (gsum*gsum/(n*n)));
  gsum /= (n*n);
}




