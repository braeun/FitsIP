/********************************************************************************
 *                                                                              *
 * Mathematical functions                                                       *
 *                                                                              *
 * modified: 14.11.2008                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) 2005-2008 by Harald Bräuning.   All Rights Reserved.           *
 ********************************************************************************
 * This program is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU General Public License                  *
 * as published by the Free Software Foundation; either version 2.1             *
 * of the License, or (at your option) any later version.    	                  *
 *                                                                              *
 * This program is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * General Public License for more details.                                     *
 *                                                                              *
 * You should have received a copy of the GNU General Public                    *
 * License along with this library; if not, write to the Free Software          *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   *
 ********************************************************************************/

#ifndef HMATH_FUNCTIONS_H
#define HMATH_FUNCTIONS_H

#include "dllapi.h"
#include <stdlib.h>
#include <math.h>
#include <vector>

#ifndef M_PI
#define M_PI   3.1415926535897932384626433       /* pi */
#endif

namespace HMath {

/**  Order the double values */
extern HMATHAPI void OrderMinMax(double &min, double &max);
/**  Order the double values */
extern HMATHAPI void OrderMinMax(int &min, int &max);
/**  Polynom of order n - 1 */
extern HMATHAPI double Polynom(double x, int n, double par[], double d[]);
/**  exp-function plus background */
extern HMATHAPI double Exponential(double x, int n, double par[], double d[]);
/**  Gauss-function + background */
extern HMATHAPI double Gaussian(double x, int n, double par[], double d[]);
/**  Simple Gauss-function */
extern HMATHAPI double Gaussian1(double x, int n, double par[], double d[]);
/**  2 dimensional Gauss-function + background */
extern HMATHAPI double Gaussian2D(double x, double y, int n, double par[], double d[]);
/**  Sum of Gauss-functions + constant background */
extern HMATHAPI double GaussianN(double x, int n, double par[], double d[]);
/**  Fit y = a + b * x to given data in vectors x and y. Return chi square and the errors of a and b */
extern HMATHAPI double LinFit(const std::vector<double> &x, const std::vector<double> &y, double *a, double *b, double *erra=NULL, double *errb=NULL);
/**  Fit y = a + b * x to given data in vectors x and y with the standard deviations sigma. Return chi square and the errors of a and b */
extern HMATHAPI double WeightedLinFit(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &sigma, double *a, double *b, double *erra=NULL, double *errb=NULL);
/**  Compute ln(Gamma(x)) */
extern HMATHAPI double GammaLn(double x);
/**  Compute incomplete gamma function P(a,x) */
extern HMATHAPI double GammaP(double a, double x);
/**  Compute incomplete gamma function Q(a,x) = 1 - P(a,x) */
extern HMATHAPI double GammaQ(double a, double x);

} /* namespace */

#endif
