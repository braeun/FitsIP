/********************************************************************************
 *                                                                              *
 * FitsIP - star detection class                                                *
 *                                                                              *
 * modified: 2025-02-11                                                         *
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

#ifndef FINDSTARS_H
#define FINDSTARS_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <fitsbase/star.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class FindStars: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)
public:
  FindStars();
  ~FindStars();

  virtual bool createsNewImage() const override;

  virtual std::vector<std::shared_ptr<FitsObject>> getCreatedImages() const override;

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  std::vector<Star> findStars(std::shared_ptr<FitsImage> image);

  void starAxes(std::shared_ptr<FitsImage> image, const QRect& box, double sky,
                double *xc, double *yc, double *fwhm, double *xwidth, double *ywidth, int maxiter);

private:

  class Gaussian
  {
  public:
    Gaussian(double fwhm);
    Gaussian(const Gaussian&) = default;
    Gaussian(Gaussian&&) = default;
    uint32_t n;
    std::vector<double> data;
    double gsum;		/* sum of terms in gaussian */
    double gsum2;		/* sum of squares in terms in gaussian */
    double gnum;		/* denominator of expression for H */
  };

  std::shared_ptr<FitsImage> convolve(std::shared_ptr<FitsImage> image, double fwhm);
  double do_gauss(std::shared_ptr<FitsImage> image, uint32_t x, uint32_t y, const Gaussian& gauss);
  double find_skysig(std::shared_ptr<FitsImage> image, double rough_sig);
  bool is_peak(std::shared_ptr<FitsImage> image, uint32_t xc, uint32_t yc);
  void calc_box(int x, int y, int w, int h, int *tsx, int *tsy, int wsize2, int hsize2);
  void star_axes(std::shared_ptr<FitsImage> image, int tsx, int tsy, int w, int h, double sky,
                 double *xc, double *yc, double *fwhm, double *xwidth, double *ywidth, int maxiter);
  double do_fwhm(std::shared_ptr<FitsImage> image, int sx, int sy, int w, int h, double xc, double yc, double sky);
  double get_frac(double larger, double smaller, double middle);
  bool validFWHM(double fwhm);
  void markpix(std::shared_ptr<FitsImage> image, double xc, double yc, double fwhm);
  double hotness(std::shared_ptr<FitsImage> image, uint32_t x, uint32_t y);
  double sharpness(std::shared_ptr<FitsImage> image, uint32_t x, uint32_t y);


  std::vector<Star> stars;
  int c_biton;
  int c_abovesig;
  int c_peak;
  int c_1validfwhm;
  int c_2validfwhm;
  int c_movedaway;
  int c_round;
  int c_sharp;
  int c_hot;
  int c_star;
  double skysig;          /* std. dev. of sky noise */
  double minsig;          /* minimum # of std. dev. above which we look for stars */
  double starfwhm;        /* default fwhm of stellar features */
  double minfwhm;         /* min allowed stellar profile fwhm */
  double maxfwhm;         /* max ditto */
  double minround;        /* min allowed 'round' criterion for a valid star */
  double maxround;        /* max ditto */
  double minsharp;        /* min allowed 'sharp' criterion for a valid star */
  double maxsharp;        /* max ditto */
  double cminsharp;       /* min 'sharp' criterion, when convolving */
  double cmaxsharp;       /* max ditto */
  double minhot;          /* min allowed 'hotness' criterion for a valid star */
  double maxhot;          /* max ditto */
  double movemax;         /* max difference between brightest pixel and center of light */
  double sky;             /* background sky value */
  bool quickflag;         /* if true, no guassian convolution is done */
  uint32_t peakrad;       /* peak radius (in pixel) for search algorithms */
  int tinysize;           /* size of area on which to perform analysis (>= maxfwhm) */
  int maxiter;
  std::vector<uint8_t> bitmap;        /* bitmap to flag pixel as used */
  std::shared_ptr<FitsImage> conv_img;

};

#endif // FINDSTARS_H
