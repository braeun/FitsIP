/********************************************************************************
 *                                                                              *
 * 1D Fit function                                                              *
 *                                                                              *
 * modified: 28.02.2009                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) 2005-2009 by Harald Bräuning.   All Rights Reserved.           *
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

#ifndef HMATH_FIT1D_H
#define HMATH_FIT1D_H

#include "dllapi.h"
#include "FitPar.h"
#include "Matrix.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>


/* Defines for predefined 1d fit functions */
#define FIT_POLYNOM      0
#define FIT_EXPONENTIAL  1
#define FIT_GAUSSIAN     2
#define FIT_GAUSSIAN_BKG 3
#define FIT_GAUSSIAN_N   4
#define FIT_COSINE       5
#define FIT_SINE         6

#define FIT_MAXFUNCTION  7



#define FIT_LAMBDAINI	1.e-3	  /* Initial value for lambda */
#define FIT_LAMBDAMIN	0.	    /* Min lambda we need for a successful fit */
#define FIT_LAMBDAMAX	1.e+5	  /* Max lambda we use to stop divergence */
#define FIT_ITERMAX 	100	    /* Max itarations we allow */
#define FIT_CHIEND1	  1.e-5   /* Chi-square end criterium of fit */
#define FIT_CHIEND2	  1.e-4   /* Chi-square end criterium of fit */


namespace HMath {


/**  Least-Squares fit to a non-linear function:
 *
 * This algorithm uses the 'Gradient-expansion Algorithm' of Marquardt.
 * It starts with a 'Gradient-Fit' (large lambda) und ends with a
 * 'Analytical solution by linearisation of the fit function' (small lambda).
 *
 * If one alpha(i,i) == 0 the problem is normally not solvable. To avoid this
 * a small value != 0 is substituted to continue the fit.
 *
 * Algorithm according to Press et al. Numerical Recipes in C
 * some code fragments by K.Huber, Strahlenzentrum, Univ. Giessen.
 */
class HMATHAPI Fit {
 public:
 	Fit(void);
	~Fit(void);

	/** Return true if an error occured */
 	bool IsError(void) const                                           { return _error != ""; }
 	/** Return the error message or an empty string if no error occured. */
 	const char *GetErrorMessage(void) const                            { return _error.c_str(); }
 	/** Calculate weights using statistical errors if @a b is true. Use equal weights otherwise */
 	void SetStatError(bool b)                                          { _stat = b; }
 	/** Return true if weights are calculated using statistical errors and false if equal weights are used. */
 	bool IsStatError(void)                                             { return _stat; }
 	/** Set the data points @a x and @a y to be fitted */
 	void SetData(const std::vector<double> &x, const std::vector<double> &y)     { _x = x; _y = y; }
 	/** Set the starting parameters @a p */
 	void SetPar(const std::vector<FitPar> &p)                          { _par = p; }
 	/** Return the fitted parameters */
 	const std::vector<FitPar> &GetPar(void) const                      { return _par; }
 	/** Return the minimum number of parameters needed for the set fit function */
 	int GetMinimumNrPar(void) const                                    { return _minpar; }
 	/** Get the description of the fit function */
 	const char *GetFunctionDescription(void) const                     { return _fname.c_str(); }
 	/** Return the covariance matrix of the fit */
 	const Matrix &GetCovarianceMatrix(void) const                   { return _covar; }
 	/** Set the maximum number of iterations to perform */
 	void SetNrIterations(int n)                                        { _niter = n; }
 	/** Return the maximum number of iterations */
 	int GetNrIterations(void) const                                    { return _niter; }
 	/** Set the absolute chi square end criterium */
 	void SetChiEndAbsolute(double v)                                   { _chiend2 = v; }
 	/** Return the absolute chi square end criterium */
 	double GetChiEndAbsolute(void) const                               { return _chiend2; }
 	/** Set the relative chi square end criterium */
 	void SetChiEndRelative(double v)                                   { _chiend1 = v; }
 	/** Return the relative chi square end criterium */
 	double GetChiEndRelative(void) const                               { return _chiend1; }
 	/** Set the maximum lambda to avoid divergence */
 	void SetLambdaMax(double v)                                        { _lambdamax = v; }
 	/** Get the maximum lambda to avoid divergence */
 	double GetLambdaMax(void) const                                    { return _lambdamax; }

	/** Set an internal fit function */
	bool SetFunction(int);
	/** Set an external fit function */
	bool SetFunction(int,double (*f)(double, const std::vector<FitPar>&, double*), int, const char *);
	/** Perform the fit and return the chi2 value. If an error occured, a chi2 < 0 is returned.  */
	double Execute(void);
	/** Calculate y value at @a x for fitted function */
	double CalculateY(double x);

 	/** Return the number of internally defined fit functions */
 	static int GetNrInternalFunctions(void);
	/** Return the equation string for the internally defined fit function with index @a i */
 	static const char *FunctionString(int);
	/** Return the description for the internally defined fit function with index @a i */
 	static const char *GetFunctionDescription(int);
  /**
   * Polynom of arbitrary order (the number of parameters determines the order of the polynom):
   * \f[
   *   a_0 + a_1 x + a_2 x^2 + \ldots
   * \f]
   */
 	static double Polynom(double, const std::vector<FitPar>&, double *d=NULL);
	/**
	 * exp-function plus background
	 * \f[
	 *   a_0 + a_1 \exp(a_2 x)
	 * \f]
	 */
 	static double Exponential(double, const std::vector<FitPar>&, double *d=NULL);
	/**
	 * Gauss-function + background
	 * \f[
	 *		a_0 + \frac{a_1}{\sqrt{2\pi}a_3}\exp(-\frac{1}{2}\frac{(x-a_2)^2}{a_3^2})
	 * \f]
	 */
 	static double Gaussian(double, const std::vector<FitPar>&, double *d=NULL);
	/**
	 * Simple Gauss-function
	 * \f[
	 *   \frac{a_0}{\sqrt{2\pi}a_2}\exp(-\frac{1}{2}\frac{(x-a_1)^2}{a_2^2})
	 * \f]
	 */
 	static double Gaussian1(double, const std::vector<FitPar>&, double *d=NULL);
	/**
	 * sum of gaussians plus background (the number of parameters determines the number of
	 * gaussians used):
	 * \f[
	 *		a_0 + \frac{a_1}{\sqrt{2\pi}a_3}\exp(-\frac{1}{2}\frac{(x-a_2)^2}{a_3^2}) +
	 *          \frac{a_4}{\sqrt{2\pi}a_6}\exp(-\frac{1}{2}\frac{(x-a_5)^2}{a_6^2}) + \ldots
	 * \f]
	 */
 	static double GaussianN(double, const std::vector<FitPar>&, double *d=NULL);
	/**
	 * Sine-function + background
	 * \f[
	 *		a_0 + a_1\sin(a_2 x + a_3)
	 * \f]
	 */
 	static double Sine(double, const std::vector<FitPar>&, double *d=NULL);
	/**
	 * Cosine-function + background
	 * \f[
	 *		a_0 + a_1\cos(a_2 x + a_3)
	 * \f]
	 */
 	static double Cosine(double, const std::vector<FitPar>&, double *d=NULL);

 private:
 	double Fit1(double);

 	bool _stat;								/* if true, use statistical weights */
 	int _niter;               /* maximum number of iterations */
 	double _lambdamax;        /* maximum lambda to stop divergence */
 	double _chiend1;          /* chi square end criterium (relative) */
 	double _chiend2;          /* chi square end criterium (absolute) */
 	std::vector<double> _x;
 	std::vector<double> _y;
 	std::vector<FitPar> _par;
 	std::string _fname;
 	int _fid;
 	int _minpar;
	double (*_f)(double, const std::vector<FitPar>&, double*);
	double *_deriv;
	double *_beta;
	Matrix _covar;
	Matrix _alpha;
	std::string _error;

	static const char *_description[];
	static const char *_equation[];
};

} /* namespace */

#endif
