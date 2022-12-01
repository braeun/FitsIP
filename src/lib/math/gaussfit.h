/******************************************************************************
 *                                                                            *
 * FitsIP - gaussian fit                                                      *
 *                                                                            *
 * modified: 2022-11-26                                                       *
 *                                                                            *
 ******************************************************************************
 * Copyright (C) Harald Braeuning                                             *
 ******************************************************************************
 *                                                                            *
 * Copyright (c) 1996 Michael Richmond and Richard Treffers                   *
 *                                                                            *
 *                    This software may be copied and distributed for         *
 *                    educational, research and not for profit services       *
 *                    provided that this copyright and statement are          *
 *                    included in all such copies.                            *
 *                                                                            *
 *****************************************************************************/
#ifndef GAUSSFIT_H
#define GAUSSFIT_H

#include "../fitstypes.h"

class GaussFit
{
public:
  GaussFit();

  void setMaxIteration(int max);

  /**
   * @brief Gauss fit
   *
   * find the gaussian that best fits the given x[] and y[] data;
   * return the amplitude (A), center (m) and width (s) of the gaussian,
   * where
                                   (x - m)^2
                  gaussian = A exp(---------)
                                     2*s^2

   * and the formal chisq uncertainty in the fit.  Note that if the
   * user doesn't know (or care) what the sigma in each
   * y[] point is, he can pass values of 1.0 in each sig[] and
   * then ignore the chisq.  Only "real" values for sig[] produce
   * a meaningful chisq, but this function always returns it.
   *
   * @param x
   * @param y
   * @param sig
   * @param ndata
   * @return -1 if the user passes fewer than 3 points, or if
   *         the parameters don't converge within the maximum number of
   *         iterations.  If there is convergence, return 0
   */
  int fit(ValueType x[], ValueType y[], ValueType sig[], int ndata);


  /**
   * @brief Fast analytical fit
   *
   *               We want to find the gaussian that best fits
   *               the given x[] and y[] data;
   *               return the amplitude (A), center (m) and width (s)
   *               of the gaussian, where
   *
   *                               (x - m)^2
   *               y(x)    = A exp(---------)
   *                                 2*s^2
   *
   *              This routine tries to be quick by using a trick:
   *              it takes the logarithm of both sides, reducing
   *              the problem to one of finding the coefficients
   *              of a quadratic equation.  See Guo, "A simple algorithm
   *              for fitting a gaussian function", IEEE Signal
   *              Processing Magazine, vol 23, p 134 (2011),
   *              and Pastuchova and Zapkopcan, "Comparison of algorithms
   *              for fitting a gaussian function used in testing
   *              smart sensors", Journal of Electrical Engineering,
   *              vol 66, number 3, p 178 (2015).
   *
   *              This method can fail if the determinant of a
   *              matrix is zero.  In that case, it will return 1.
   *
   *              At the moment, the "chisq" output argument is simply
   *              set to 1.0.
   *
   * @param x array of ndata x values
   * @param y array of ndata y values
   * @param ndata number of elements in x, y
   * @return 0 if ok or 1 on error
   */
  int fastFit(ValueType x[], ValueType y[], int ndata);

  double getCenter() const;

  double getWidth() const;

  double getAmplitude() const;

  double getChiSq() const;

private:

  void guess_params(ValueType* x, ValueType* y, int ndata, ValueType *amp, ValueType *center, ValueType *width);
  ValueType find_chisq(ValueType* x, ValueType* y, ValueType* sig, ValueType* sig2, int ndata, ValueType amp, ValueType center, ValueType width);

  bool pixelCenter;
  int maxIter;
  bool allowEarlyTermination;
  ValueType center;
  ValueType width;
  ValueType amp;
  ValueType chisq;
};

#endif // GAUSSFIT_H
