/********************************************************************************
 *                                                                              *
 * Mathematical functions                                                       *
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

#ifndef HMATH_FIT2D_H
#define HMATH_FIT2D_H

#include "dllapi.h"
#include "FitPar.h"
#include "Matrix.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>


/* Defines for predefined 2d fit functions */
#define FIT2D_GAUSSIAN      0
#define FIT2D_MAXFUNCTION   1

namespace HMath {

/**  Least-Squares fit to a non-linear 2 dimansional function:
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
class HMATHAPI Fit2D {
 public:
 	Fit2D(void);
 	~Fit2D(void);

	/** Return true if an error occured */
 	bool IsError(void) const                                           { return _error != ""; }
 	/** Return the error message or an empty string if no error occured. */
 	const char *GetErrorMessage(void) const                            { return _error.c_str(); }
 	/** Calculate weights using statistical errors if @a b is true. Use equal weights otherwise */
 	void SetStatError(bool b)                                          { _stat = b; }
 	/** Return true if weights are calculated using statistical errors and false if equal weights are used. */
 	bool IsStatError(void)                                             { return _stat; }
 	/** Set the data points @a x, @a y and @a z to be fitted */
 	void SetData(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z)     { _x = x; _y = y; _z = z; }
 	/** Set the starting parameters @a p */
 	void SetPar(const std::vector<FitPar> &p)                          { _par = p; }
 	/** Return the fitted parameters */
 	const std::vector<FitPar> &GetPar(void) const                      { return _par; }
 	/** Return the minimum number of parameters needed for the set fit function */
 	int GetMinimumNrPar(void) const                                    { return _minpar; }
 	/** Get the description of the fit function */
 	const char *GetFunctionDescription(void) const                     { return _fname.c_str(); }
 	/** Return the covariance matrix of the fit */
 	const Matrix &GetCovarianceMatrix(void) const                      { return _covar; }
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
	bool SetFunction(int,double (*f)(double, double, const std::vector<FitPar>&, double*), int, const char*);
	/** Perform the fit and return the chi2 value. If an error occured, a chi2 < 0 is returned.  */
	double Execute(void);
	/** Calculate z value at @x and @a y for fitted function */
	double CalculateZ(double x, double y);

 	/** Return the number of internally defined fit functions */
 	static int GetNrInternalFunctions(void);
	/** Return the equation string for the internally defined fit function with index @a i */
 	static const char *FunctionString(int);
	/** Return the description for the internally defined fit function with index @a i */
 	static const char *GetFunctionDescription(int);
	/**
	 * 2 dimensional Gauss-function + background
	 */
	static double Gaussian2D(double x, double y, const std::vector<FitPar> &par, double *d=NULL);

 private:
 	double Fit1(double);

 	bool _stat;								/* if true, use statistical weights */
 	int _niter;               /* maximum number of iterations */
 	double _lambdamax;        /* maximum lambda to stop divergence */
 	double _chiend1;          /* chi square end criterium (relative) */
 	double _chiend2;          /* chi square end criterium (absolute) */
 	std::vector<double> _x;
 	std::vector<double> _y;
 	std::vector<double> _z;
 	std::vector<FitPar> _par;
 	std::string _fname;
 	int _fid;
 	int _minpar;
	double (*_f)(double, double, const std::vector<FitPar>&, double*);
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
