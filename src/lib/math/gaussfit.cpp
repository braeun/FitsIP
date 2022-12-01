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

  /* a routine that fits a gaussian to some data, without using ANY
     Numerical Recipes routines at all.  It makes an initial guess as
     the parameters, then iterates, trying to minimize chi-square for
     each parameter independently in each iteration.  Seems to work
     pretty well, even given data only on one side of the gaussian peak.

     if it the fit doesn't converge, it returns SDSS_ERROR and the values
     of the parameters are left untouched.  If it does converge, the
     function returns SDSS_OK and the values are placed into the
     passed parameters.   MWR 12/7/92. */


  /* 12/8/92
   *           - changed name of function from 'gauss_fit' to 'gauss_fit'
   *           - now include "utils.h"
   *               ... MWR
   */

  /* 12/11/92
   *           - modified version for non-SDSS use in XVista.
   *               ... MWR
   8/24/93 - check for maximum value of exp function -rrt
   */

  /*
   * Modified to use a fixed stepsize in fitting center position parameter,
   *    instead of using a stepsize based on value of center position,
   *    when acting on stars.  Still uses a fractional stepsize
   *    when fitting sky values.
   *
   * Also, removed the possibility of early termination from the
   *    fitting loop: will now always run for MAXITER iterations.
   *    If you want to speed up the program, #define ALLOW_EARLY_TERMINATION.
   *
   *    MWR 6/14/2013
   */

  /*
   * Added a new routine, "gauss_fit_fast()", which uses a non-iterative
   *    matrix-inversion method to find the parameters of a gaussian.
   *    Given perfect data, it works and is MUCH faster than the
   *    iterative method of "gauss_fit()" -- but in noisy data such
   *    as typical Tomo-e images, it produces garbage output on almost
   *    all stars.
   *
   * So, it's not reliable for that purpose, but I'm leaving it in this
   *    file in case it might come in handy for a very different sort
   *    of dataset.
   *
   *    MWR 6/9/2019
   *
   * Added a new argument, "maxiter", to gauss_fit(); it takes the place
   *    of the old #define MAXITER.
   *
   *    MWR 6/15/2019
   */

#include "gaussfit.h"
#include <cmath>
#include <new>
#include <QDebug>


#define TINY  1.0e-4	/* amount to which chisq should converge */
                      /*   and also min. fractional amount by which */
                      /*   gaussian parameters allowed to change */

#define MAX_ARG 10.	/* largest argument for exp function */


#define FACTOR 0.1	/* fraction of parameters used as a step size */
                    /*   to determine derivative empirically      */

/*
 * If we are fitting a stellar centroid, and the OLD_CENTER_ALGORITHM
 *  value is at its default value of 0, then we use the following as
 *  the initial stepsize for changes in the center position
 *  of the gaussian we are fitting; it is appropriate for determining
 *  the central position of a star in 1-D.
 * If OLD_CENTER_ALGORITHM has been set to 1, then we use the same
 *  initial stepsize for center position as for width or amplitude:
 *  FACTOR times the initial guess.
 */
#define PIXEL_STEPSIZE  0.020
/*
 * if OLD_CENTER_ALGORITHM is not set to 1, so we are using a fixed
 * stepsize as defined above for center position of gaussian,
 * then use the following absolute value as termination criterion
 * for iterating over changes to center position.
 */
#define PIXEL_CONVERGE   1.0e-4

/*
 * The function "fastFit()" uses a matrix-based algorithm
 *   to fit a gaussian to 1-D arrays of measurements.  If the
 *   determinant of the matrix is smaller than this value,
 *   the routine will declare failure.
 */
#define DET_TINY  1.0e1

GaussFit::GaussFit():
  pixelCenter(true),
  maxIter(20),
  allowEarlyTermination(false)
{
}

void GaussFit::setMaxIteration(int max)
{
  maxIter = max;
}

int GaussFit::fit(ValueType *x, ValueType *y, ValueType *sig, int ndata)
{
  if (ndata < 3)
  {
    qWarning() << "gauss_fit: passed fewer than three data points";
    return -1;
  }

  /* make some initial guesses to the parameters here */
  ValueType a[3];
  guess_params(x,y,ndata,&(a[0]),&(a[1]),&(a[2]));
  double guess_a1 = a[1];

  ValueType alpha[3];
  for (int i=0;i<3;i++)
  {
    if (pixelCenter && (i == 1))
    {
      alpha[i] = PIXEL_STEPSIZE;
    }
    else
    {
      alpha[i] = fabs(a[i]) * FACTOR;
    }
  }

  ValueType* sig2 = new (std::nothrow) ValueType[ndata];
  if (sig2)
  {
    for (int i=0;i<ndata;i++)
    {
      sig2[i] = sig[i] * sig[i];
    }
  }

  /* first, we find chisq given the guessed parameters */
  ValueType chi = find_chisq(x,y,sig,sig2,ndata,a[0],a[1],a[2]);
  ValueType oldchisq = chi;

  int iter = 0;
#ifdef DEBUG
  printf("iter %2d: a[0]=%-9.4e, a[1]=%-11.6e, a[2]=%-9.4e  chi=%-9.4e\n",
        iter, a[0], a[1], a[2], chi);
#endif

  ValueType newa[3];
  ValueType deriv[3];
  ValueType sign[3];
  do
  {

  /* now, find the change in chisq w.r.t. change in each of the three
     parameters.  use alpha[i] as the change in parameter[i] used to find
     the derivative empirically */
    for (int i=0;i<3;i++)
    {
      newa[i] = a[i];

#ifdef DEBUG2
      printf("  param %d: ", i);
#endif
      if (alpha[i] < TINY)
      {
#ifdef DEBUG2
      printf("  skipping\n");
#endif
        continue;
      }
      a[i] -= alpha[i];
      ValueType x1 = find_chisq(x,y,sig,sig2,ndata,a[0],a[1],a[2]);
#ifdef DEBUG2
      printf("%-9.4e %-9.4e  ", a[i], x1);
#endif
      a[i] += 2.0*alpha[i];
      ValueType x2 = find_chisq(x,y,sig,sig2,ndata,a[0],a[1],a[2]);
#ifdef DEBUG2
      printf("%-9.4e %-9.4e  ", a[i], x2);
#endif
      a[i] -= alpha[i];		/* put a[i] back to where it started */
      deriv[i] = (x2 - x1)/(2.0*alpha[i]);

#ifdef DEBUG2
      printf("%-9.4e ", deriv[i]);
#endif

      sign[i] = ((deriv[i] > 0) ? 1 : -1);

      /* now, we want to make the derivative zero by making (x2 - x1)
         equal to zero.  So, we want to move a[i] in the direction
         OPPOSITE the deriv, by some amount - let's say, alpha[i].
         If the deriv doesn't change sign on either side of the
         current a[i], then leave the amount alpha[i] alone and
         use a new value for a[i].  If it does change, we're close
         to a minimum; therefore, then cut alpha[i] by a factor of 4
         and don't change a[i] in this iteration. */
      if (fabs(deriv[i]) > TINY)
      {
        if (((deriv[i] < 0) && (x2 < chi)) || ((deriv[i] > 0) && (x1 < chi)))
        {
          newa[i] = a[i] - sign[i]*alpha[i];
#ifdef DEBUG2
          printf("new %9.4e\n", newa[i]);
#endif
          if (pixelCenter && (i == 1))
          {
            alpha[i] = PIXEL_STEPSIZE;
          }
          else
          {
            alpha[i] = fabs(newa[i])*FACTOR;
          }
        }
        else
        {
          newa[i] = a[i];	/* leave it as is... */
#ifdef DEBUG2
          printf("div %9.4e\n", newa[i]);
#endif
          alpha[i] *= 0.25;
        }
      }
      else
      {
#ifdef DEBUG2
        printf("sam %9.4e\n", a[i]);
#endif
      }

    }
    for (int i=0;i<3;i++) a[i] = newa[i];

    chi = find_chisq(x,y,sig,sig2,ndata,a[0],a[1],a[2]);

    iter++;
#ifdef DEBUG
    printf("iter %2d: a[0]=%-9.4e, a[1]=%-11.6e, a[2]=%-9.4e  chi=%-9.4e\n",
        iter, a[0], a[1], a[2], chi);
      printf("       alpha[0]=%-13.4f, alpha[1]=%-13.4f, alpha[2]=%-13.4f \n",
            alpha[0], alpha[1], alpha[2]);

#endif

    /*
     * if one adds the following check, one will sometimes
     *   terminate before maxiter iterations; may save 50% time,
     *   may yield very marginally worse results.
     */
    if (allowEarlyTermination && fabs(chi - oldchisq) < TINY) break;
    oldchisq = chi;
  }
  while (iter < maxIter);

  /* free up the "sig2" array, if we allocated it */
  if (sig2) delete [] sig2;

//  /* If we didn't converge, just return with best numbers so far */
//  /* Maybe we want to print out a warning message here? */
//  if (iter == maxiter)
//  {
//    a[0] = a[0];    /* to placate compiler */
//  }

  amp = a[0];
  center = a[1];
#ifdef DEBUG
  printf("   orig a1 %12.4f  final %12.4f  delta %8.4f \n", guess_a1, *center, (*center - guess_a1));
#endif

  /* note that my "gauss" function doesn't use the "2*sigma^2", so we
     have to divide here by sqrt(2) */
  width = a[2] / sqrt(2.0);
  chisq = chi;

  return 0;

}

int GaussFit::fastFit(ValueType *x, ValueType *y, int ndata)
{
  /*
   * allocate space for local arrays
   *
   * In theory, can speed things up by checking to see if
   *   this routine has been run previously with arrays of the
   *   same size; if so, just re-use them.
   */
  ValueType* x2 = new ValueType[ndata];
  ValueType* x3 = new ValueType[ndata];
  ValueType* x4 = new ValueType[ndata];
  ValueType* yoff = new ValueType[ndata];
  ValueType* y2 = new ValueType[ndata];
  ValueType* lny = new ValueType[ndata];

  /*
   * find the minimum value of y[]; we'll add a constant to all values
   * of y before performing calculations like so:
   *
   *              yoff[i]  =  1.0 + (y[i] - ymin)
   *
   * to avoid taking log of negative or zero.
   */
  ValueType ymin = y[0];
  for (int i=1;i<ndata;i++)
  {
    if (y[i] < ymin)
    {
      ymin = y[i];
    }
  }
#ifdef DEBUG6
  printf(" ymin is %9.2f \n", ymin);
#endif
  for (int i=0;i<ndata;i++)
  {
    yoff[i] = 1.0 + (y[i] - ymin);
#ifdef DEBUG6
    printf("  new yoff[%5d]  %9.2f \n", i, yoff[i]);
#endif
  }


  /*
   * pre-compute some quantities we'll use repeatedly while building
   * matrix elements.
   */
  for (int i=0;i<ndata;i++)
  {
    x2[i] = x[i]*x[i];
    x3[i] = x[i]*x2[i];
    x4[i] = x[i]*x3[i];
    y2[i] = yoff[i]*yoff[i];
    lny[i] = log(yoff[i]);
#ifdef DEBUG6
    printf("  i %5d  x2 %12.2f x3 %12.2f x4 %12.2f y2 %12.2f lny %9.4f \n",
          i, x2[i], x3[i], x4[i], y2[i], lny[i]);
#endif
  }

  /*
   * We are going to set up a matrix equation which will allow us
   * to solve for a 3-element vector, (a, b, c).  Once we have those
   * values, we can use them to compute the parameters of the gaussian.
   *
   *                         ( a )       ( Q1 )
   *            matrix M  *  ( b )  =    ( Q2 )
   *                         ( c )       ( Q3 )
   *
   * where "M" is a 3x3 matrix based on combinations of the x and y
   * values, and "Q" is a 3-element vector based on a different
   * combination of x and y values.
   */

  /* compute elements of matrix M and vector Q */
  ValueType M[3][3], Q[3], Minv[3][3];
  for (int i=0;i<3;i++)
  {
    Q[i] = 0.0;
    for (int j=0;j<3;j++)
    {
      M[i][j] = 0.0;
      Minv[i][j] = 0.0;
    }
  }
  for (int i=0;i<ndata;i++)
  {
    M[0][0] += y2[i];
    M[1][0] += x[i]*y2[i];
    M[2][0] += x2[i]*y2[i];

    M[0][1] = M[1][0];
    M[1][1] = M[2][0];
    M[2][1] += x3[i]*y2[i];

    M[0][2] = M[2][0];
    M[1][2] = M[2][1];
    M[2][2] += x4[i]*y2[i];

    Q[0] += y2[i]*lny[i];
    Q[1] += x[i]*y2[i]*lny[i];
    Q[2] += x2[i]*y2[i]*lny[i];
  }

#ifdef DEBUG6
  printf("  M follows \n");
  printf("  %12.4e  %12.4e  %12.4e \n", M[0][0], M[0][1], M[0][2]);
  printf("  %12.4e  %12.4e  %12.4e \n", M[1][0], M[1][1], M[1][2]);
  printf("  %12.4e  %12.4e  %12.4e \n", M[2][0], M[2][1], M[2][2]);
  printf("  Q follows \n");
  printf("  %12.4e  %12.4e  %12.4e \n", Q[0], Q[1], Q[2]);
  printf(" \n");
#endif

  /* Now, we will go through 5 steps to compute the inverse of matrix M */

  /* Step 1: compute determinant of matrix M.
   *         If it's zero, or very small, then return with error.
   */
  ValueType det;

  det =  M[0][0]*(M[1][1]*M[2][2] - M[1][2]*M[2][1]);
  det -= M[0][1]*(M[1][0]*M[2][2] - M[1][2]*M[2][0]);
  det += M[0][2]*(M[1][0]*M[2][1] - M[1][1]*M[2][0]);
#ifdef DEBUG6
  printf(" fit_gauss_fast: det is %9.4e \n", det);
#endif
  if (fabs(det) < DET_TINY) {
#ifdef DEBUG6
    fprintf(stderr, "fit_gauss_fast: fabs det %9.4e < %9.4e so fail \n",
          det, DET_TINY);
#endif
    return(1);
  }

  /*
   * Step 2: Create a new matrix, P, in which each element
   *         is the determinant of the minor of each element in M.
   */
  ValueType P[3][3];
  P[0][0] = M[1][1]*M[2][2] - M[1][2]*M[2][1];
  P[0][1] = M[1][0]*M[2][2] - M[1][2]*M[2][0];
  P[0][2] = M[1][0]*M[2][1] - M[1][1]*M[2][0];

  P[1][0] = M[0][1]*M[2][2] - M[0][2]*M[2][1];
  P[1][1] = M[0][0]*M[2][2] - M[0][2]*M[2][0];
  P[1][2] = M[0][0]*M[2][1] - M[0][1]*M[2][0];

  P[2][0] = M[0][1]*M[1][2] - M[0][2]*M[1][1];
  P[2][1] = M[0][0]*M[1][2] - M[0][2]*M[1][0];
  P[2][2] = M[0][0]*M[1][1] - M[0][1]*M[1][0];

  /*
   * Step 3: Invert the signs of 4 elements of P
   */
  P[0][1] = 0.0 - P[0][1];
  P[1][0] = 0.0 - P[1][0];
  P[1][2] = 0.0 - P[1][2];
  P[2][1] = 0.0 - P[2][1];

  /*
   * Step 4: Set Minv to the transpose of P
   */
  Minv[0][0] = P[0][0];
  Minv[0][1] = P[1][0];
  Minv[0][2] = P[2][0];

  Minv[1][0] = P[0][1];
  Minv[1][1] = P[1][1];
  Minv[1][2] = P[2][1];

  Minv[2][0] = P[0][2];
  Minv[2][1] = P[1][2];
  Minv[2][2] = P[2][2];

  /*
   * Step 5: Divide all elements of Minv by the determinant
   */
  Minv[0][0] /= det;
  Minv[0][1] /= det;
  Minv[0][2] /= det;

  Minv[1][0] /= det;
  Minv[1][1] /= det;
  Minv[1][2] /= det;

  Minv[2][0] /= det;
  Minv[2][1] /= det;
  Minv[2][2] /= det;

#ifdef DEBUG6
  printf("  Minv follows \n");
  printf("  %12.4e  %12.4e  %12.4e \n", Minv[0][0], Minv[0][1], Minv[0][2]);
  printf("  %12.4e  %12.4e  %12.4e \n", Minv[1][0], Minv[1][1], Minv[1][2]);
  printf("  %12.4e  %12.4e  %12.4e \n", Minv[2][0], Minv[2][1], Minv[2][2]);
#endif

#ifdef DEBUG6
  /* check by multiplying the matrix M by Minv.  Do we get identity? */
  ValueType iden[3][3];
  iden[0][0] = M[0][0]*Minv[0][0] + M[0][1]*Minv[1][0] + M[0][2]*Minv[2][0];
  iden[1][0] = M[1][0]*Minv[0][0] + M[1][1]*Minv[1][0] + M[1][2]*Minv[2][0];
  iden[2][0] = M[2][0]*Minv[0][0] + M[2][1]*Minv[1][0] + M[2][2]*Minv[2][0];

  iden[0][1] = M[0][0]*Minv[0][1] + M[0][1]*Minv[1][1] + M[0][2]*Minv[2][1];
  iden[1][1] = M[1][0]*Minv[0][1] + M[1][1]*Minv[1][1] + M[1][2]*Minv[2][1];
  iden[2][1] = M[2][0]*Minv[0][1] + M[2][1]*Minv[1][1] + M[2][2]*Minv[2][1];

  iden[0][2] = M[0][0]*Minv[0][2] + M[0][1]*Minv[1][2] + M[0][2]*Minv[2][2];
  iden[1][2] = M[1][0]*Minv[0][2] + M[1][1]*Minv[1][2] + M[1][2]*Minv[2][2];
  iden[2][2] = M[2][0]*Minv[0][2] + M[2][1]*Minv[1][2] + M[2][2]*Minv[2][2];

  printf("  M*Minv follows \n");
  printf("  %12.4e  %12.4e  %12.4e \n", iden[0][0], iden[0][1], iden[0][2]);
  printf("  %12.4e  %12.4e  %12.4e \n", iden[1][0], iden[1][1], iden[1][2]);
  printf("  %12.4e  %12.4e  %12.4e \n", iden[2][0], iden[2][1], iden[2][2]);
#endif

  /*
   * Now that we have the inverse of matrix M, we can solve for the
   *   3-element vector which will give us the parameters of the
   *   gaussian.  These three elements are (a, b, c).
   */
  ValueType a = Minv[0][0]*Q[0] + Minv[0][1]*Q[1] + Minv[0][2]*Q[2];
  ValueType b = Minv[1][0]*Q[0] + Minv[1][1]*Q[1] + Minv[1][2]*Q[2];
  ValueType c = Minv[2][0]*Q[0] + Minv[2][1]*Q[1] + Minv[2][2]*Q[2];
#ifdef DEBUG6
  printf("  a %10.4f  b %10.4f  c %10.4f \n", a, b, c);
#endif

  /*
   * Extract the parameters of the gaussian from a, b, c
   *
   *        center  =  - b / 2*c
   *
   *        width   =  sqrt( -1.0 / 2*c );
   *
   *        amp     =   exp( a - [b*b/4*c] )
   *
   *  but then correct the amp for the shift we applied to all y values
   *
   *        amp     =   ymin  +  (amp - 1.0)
   */
  if ((b == 0.0) || (c == 0.0)) {
#ifdef DEBUG6
    printf("  fit_gauss_fast: b %10.4e or c %10.4e == 0, so fail \n", b, c);
#endif
    return 1;
  }
  ValueType f_center = 0.0 - (b / (2.0*c));
  if (c >= 0) {
#ifdef DEBUG6
    printf("  fit_gauss_fast: c %10.4e <= 0, so fail \n", c);
#endif
    return 1;
  }
  ValueType f_width = sqrt( -1.0 / (2.0*c));

  /* don't forget to shift the amp down by ymin */
  ValueType f_amp = exp(a - ((b*b)/(4.0*c)));
  f_amp = ymin + (f_amp - 1.0);

#ifdef DEBUG6
  printf("  f_amp %10.4lf  f_center %10.4lf  f_width %10.4lf \n",
        f_amp, f_center, f_width);
#endif

  center = f_center;
  width = f_width;
  amp = f_amp;
  chisq = 1.0;

  /*
   * could free the arrays allocated herein here, or leave for
   *   the next invocation to use
   */
  delete [] x2;
  delete [] x3;
  delete [] x4;
  delete [] yoff;
  delete [] y2;
  delete [] lny;

  return 0;
}

double GaussFit::getCenter() const
{
  return center;
}

double GaussFit::getWidth() const
{
  return width;
}

double GaussFit::getAmplitude() const
{
  return amp;
}

double GaussFit::getChiSq() const
{
  return chisq;
}






void GaussFit::guess_params(ValueType *x, ValueType *y, int ndata, ValueType *amp, ValueType *center, ValueType *width)
{
  /* guess that the amplitude is (min - max) */
  ValueType min = y[0];
  ValueType max = y[0];
  for (int i=0;i<ndata;i++)
  {
    if (y[i] < min)	min = y[i];
    if (y[i] > max)	max = y[i];
  }
  *amp = max - min;

  /* guess that the center is given by the first moment */
  ValueType sumy = 0.0;
  ValueType sumxy = 0.0;
  for (int i=0;i<ndata;i++)
  {
    sumy += y[i];
    sumxy += x[i]*y[i];
  }
  if (sumy == 0.0)
    *center = 0.0;
  else
    *center = sumxy/sumy;

  /* guess that the width is sqrt(the second moment) */
  ValueType sumxyy = 0.0;
  for (int i=0;i<ndata;i++)
  {
    sumxyy += (x[i] - *center)*(x[i] - *center)*y[i];
  }
  if (sumy == 0.0)
    *width = 1.0;
  else
  {
    *width = sumxyy/sumy;
    if (*width < 0.0)
      *width = 1.0;
    else
      *width = sqrt(*width);
  }
}

/* calculate the value of chisq given the data and the values of
   the gaussian parameters. return the chisq.  If the argument
   "sigflag" is equal to 1, then the "sig2" argument contains
   sig[i] squared - use it, to speed up calculations.  If the
   "sigflag" argument is zero, though, actually square each element
   of the "sig" array.  Slower, but necessary if we couldn't allocate
   space for the "sig2" array */
ValueType GaussFit::find_chisq(ValueType *x, ValueType *y, ValueType *sig, ValueType *sig2, int ndata, ValueType amp, ValueType center, ValueType width)
{

#ifdef DEBUG3
  printf("find_chisq: given a[0]=%-9.4e, a[1]=%-9.4e, a[2]=%-9.4e\n", amp,
    center, width);
  printf("find_chisq: given sigflag %5d \n", sigflag);
#endif

  ValueType sum = 0;
  /*
   * this is a hack, but if someone passes "width=0", we have to
   * do _something_.
   */
  if (fabs(width) < TINY) width = TINY;

  if (!sig2)
  {
    for (int i=0;i<ndata;i++)
    {
      ValueType arg = (x[i] - center)/width;
      ValueType ex;
      if (fabs(arg) < MAX_ARG)
        ex = exp(-arg*arg);
      else
        ex=0.;
      ValueType yy = amp*ex;
      sum += (yy - y[i]) * (yy - y[i]) / sig2[i];
#ifdef DEBUG3
      printf("   x=%-9.4e  y=%-9.4e yy=%-9.4e  sum=%9.4e\n", x[i], y[i], yy, sum);
#endif
    }
  }
  else
  {
    for (int i=0;i<ndata;i++)
    {
      ValueType arg = (x[i] - center)/width;
      ValueType ex;
      if (fabs(arg) < MAX_ARG)
        ex = exp(-arg*arg);
      else
        ex=0.;
      ValueType yy = amp*ex;
      sum += (yy - y[i])*(yy - y[i])/(sig[i]*sig[i]);
#ifdef DEBUG3
      printf("   x=%-9.4e  y=%-9.4e yy=%-9.4e  sum=%9.4e\n", x[i], y[i], yy, sum);
#endif
    }
  }
  return sum;
}
