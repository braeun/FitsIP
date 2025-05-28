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

#include "Fit1D.h"
#include <iostream>

using namespace std;

#define FIT_LAMBDAINI 1.e-3   /* Initial value for lambda */
#define FIT_LAMBDAMIN 0.      /* Min lambda we need for a successful fit */
#define FIT_LAMBDAMAX 1.e+5   /* Max lambda we use to stop divergence */
#define FIT_ITERMAX   100     /* Max itarations we allow */
#define FIT_CHIEND1   1.e-5   /* Chi-square end criterium of fit */
#define FIT_CHIEND2   1.e-4   /* Chi-square end criterium of fit */

namespace HMath {

const char *Fit::_description[] = {
			"Polynom",
			"Exponential with Background",
			"Gaussian",
			"Gaussian with Background",
			"N gaussian with Background",
			"Cosine with Background",
			"Sine with Background",
};


const char *Fit::_equation[] = {
			"y = a0 + a1*x + a2*x^2 + ...",
			"y = a0 + a1*exp(a2*x)",
			"y = a0/(sqrt(2pi)*a2)*exp(-0.5*((x-a1)/a2)^2)",
			"y = a0 + a1/(sqrt(2pi)*a3)*exp(-0.5*((x-a2)/a3)^2)",
			"y = a0 + a1/(sqrt(2pi)*a3)*exp(-0.5*((x-a2)/a3)^2) + ...",
			"y = a0 + a1 * cos(a2*x+a3)",
			"y = a0 + a1 * sin(a2*x+a3)",
};

/** @page fitdetails Least-Squares fit to a non-linear function
 *
 * The algorithm used here is a so called gradient-expansion algorithm by
 * Marquardt. It combaines the best features of the gradient search with the
 * method of linearizing the fitting function. The interpolation between
 * the two extremes is controlled by a factor lambda.
 * It starts with a gradient search (large lambda) und ends with a
 * linearization of the fitting function (small lambda).
 * During the fit, the parameters are changed by the values delta alpha to minimize
 * the chi square. These values are determined by solving the matrix equation
 *   \f[
 *			\beta = \delta a \cdot \alpha'
 *	 \f]
 * If N is the number of data points (index i) and n the number of fit parameters
 * indices (j and k) we have
 *   \f[
 *			\beta_k = \sum_{i=1}^{N} (\frac{1}{\sigma_i^2}(y_i - y(x_i)) \frac{\partial y(x_i)}{\partial a_k})
 *	 \f]
 *   \f[
 *      \alpha_{jk} = \sum_{i=1}^{N}(\frac{1}{\sigma_i^2}\frac{\partial y(x_i)}{\partial a_j}\frac{\partial y(x_i)}{\partial a_k})
 *	 \f]
 *   \f[
 *       \alpha'_{jk} = \alpha_{jk} \qquad for \quad j != k
 *	 \f]
 *   \f[
 *       \alpha'_{jk} = \alpha_{jk} (1 + \lambda) \qquad for \quad j = k
 *	 \f]
 * If one alpha(i,i) == 0 the problem is normally not solvable. To avoid this
 * a small value != 0 is substituted to continue the fit.
 * The increments for the parameters are then given by
 *   \f[
 *       \delta a_j = \sum_{k=1}^{n} (\beta_k \epsilon'_{jk})
 *	 \f]
 * where
 *   \f[
 *       \epsilon' = \alpha'^{-1}
 *   \f]
 * The errors of the fit parameters can be taken as
 *   \f[
 *       \sigma_{aj} = \sqrt{\epsilon'_{jj}}
 *   \f]
 *
 * If all errors are identical,i.e.
 *   \f[
 *			\sigma_i = \sigma
 *	 \f]
 * they can be extracted from the sums in the above equations. We can then write
 *   \f[
 *     \alpha' = \frac{1}{\sigma^2} \alpha''
 *   \f]
 *   \f[
 *     \beta = \frac{1}{\sigma^2} \beta''
 *   \f]
 *   \f[
 *			\beta'' = \delta a \cdot \alpha''
 *	 \f]
 *   \f[
 *       \delta a_j = \sum_{k=1}^{n} (\beta''_k \epsilon''_{jk})
 *	 \f]
 *   \f[
 *       \sigma_{aj} = \sigma \sqrt{\epsilon''_{jj}}
 *   \f]
 *
 * This corresponds to an unweighted fit, where
 *   \f[
 *			\frac{1}{\sigma_i^2} = 1
 *	 \f]
 * has been set. The common error must be known only to calculate the errors of the
 * fit parameters.
 * If the common error sigma is not known, it can be estimated by the scatter of the
 * points around the best fitted curve. It is given by the reduced chi square:
 *   \f[
 *       \sigma = \frac{\chi^2}{N-n}
 *   \f]
 *
 *
 * The algorithm is taken according to Press et al. Numerical Recipes in C with some code fragments by
 * K.Huber, Strahlenzentrum, Univ. Giessen.
 *
 * Literature:
 * - Bevington, Data Reduction and Error Analysis, p. 235 ff.
 * - Lyons, Practical Guide to Data Analysis..., p. 53 ff.
 *
 */


Fit::Fit(void)
{
	_stat = false;
	_f = NULL;
	_deriv = NULL;
	_beta = NULL;
	_niter = FIT_ITERMAX;
	_lambdamax = FIT_LAMBDAMAX;
	_chiend1 = FIT_CHIEND1;
	_chiend2 = FIT_CHIEND2;
}

Fit::~Fit(void)
{
	if (_deriv) delete [] _deriv;
	if (_beta) delete [] _beta;
}

/*
 * Return the number of internally defined fit functions (static method)
 */
int Fit::GetNrInternalFunctions(void)
{
	return FIT_MAXFUNCTION;
}

/*
 * Return the equation string for the internally defined fit function with index @a i (static method)
 */
const char *Fit::FunctionString(int i)
{
	if (i >= 0 && i < FIT_MAXFUNCTION) return _equation[i];
	return "unknown";
}

/*
 * Return the description for the internally defined fit function with index @a i (static method)
 */
const char *Fit::GetFunctionDescription(int i)
{
	if (i >= 0 && i < FIT_MAXFUNCTION) return _description[i];
	return "unknown";
}

/*
 * Set a predefined fit function
 */
bool Fit::SetFunction(int f)
{
	switch (f)
	{
		case FIT_POLYNOM:
			_f = Polynom;
			_minpar = 2;
			_fid = f;
			break;
		case FIT_GAUSSIAN:
			_f = Gaussian1;
			_minpar = 3;
			_fid = f;
			break;
		case FIT_GAUSSIAN_BKG:
			_f = Gaussian;
			_minpar = 4;
			_fid = f;
			break;
		case FIT_EXPONENTIAL:
			_f = Exponential;
			_minpar = 3;
			_fid = f;
			break;
		case FIT_GAUSSIAN_N:
			_f = GaussianN;
			_minpar = 4;
			_fid = f;
			break;
		case FIT_COSINE:
			_f = Cosine;
			_minpar = 4;
			_fid = f;
			break;
		case FIT_SINE:
			_f = Sine;
			_minpar = 4;
			_fid = f;
			break;
		default:
			return false;
	}
	_fname = _description[_fid];
	return true;
}

/*
 * Set an external fit function
 */
bool Fit::SetFunction(int id, double (*f)(double, const vector<FitPar>&, double*), int npar,  const char *txt)
{
	_f = f;
	_fid = id;
	_minpar = npar;
	_fname = txt;
	return true;
}

/*
 * Calculate y value at @a x for fitted function
 */
double Fit::CalculateY(double x)
{
	if (_f) return _f(x,_par,NULL);
	return 0;
}

/*
 * Perform the fit and return the reduced chi2 value. If an error occured, a chi2 < 0 is returned.
 */
double Fit::Execute(void)
{
	_error = "";
	if (!_f)
	{
		_error = "No fit function";
		return -1;
	}
	if (_par.size() < _minpar)
	{
		_error = "Not enough fit parameters specified";
		return -1;
	}
	if (_x.size() <= _par.size()) /* Require at least one more data point than parameters to calculate reduced chi2 */
	{
		_error = "No enough data points";
		return -1;
	}
	if (_deriv) delete [] _deriv;
	_deriv = new double[_par.size()];
	if (_beta) delete [] _beta;
	_beta = new double[_par.size()];
  _alpha = Matrix(_par.size(),_par.size());
  _covar = Matrix(_par.size(),_par.size());
  double oldchi2 = 0;
  for (unsigned int i=0;i<_x.size();i++)
  {
    double y = _f(_x[i],_par,NULL);
    double w = 1.0;
    if (_stat) w = (_y[i] <= 0) ? 0 : (1.0 / _y[i]);
    oldchi2 += (_y[i] - y) * (_y[i] - y) * w;
  }
  double lambda = FIT_LAMBDAINI;
  int n = _niter;
  while (n--)
  {
    double chi2 = Fit1(lambda);
    if (chi2 < 0) return -1.0;
    if (chi2 > oldchi2)
    {
      if (lambda >= _lambdamax)
      {
      	char err[81];
      	sprintf(err,"lambda exceeds max value (%g)",_lambdamax);
      	_error = err;
      	return -1.0;
      }
      lambda *= 10.0;
    }
    else
    {
      if (fabs(chi2-oldchi2) < _chiend2 || fabs(chi2-oldchi2)/chi2 < _chiend1)
      {
			  chi2 /= (_x.size() - _par.size());
        return chi2;
      }
      else
        lambda /= 10.0;
    }
    oldchi2 = chi2;
  }
  char err[81];
  sprintf(err,"%d iterations exceeded",_niter);
  _error = err;
  oldchi2 /= (_x.size() - _par.size());
  return oldchi2;
}

double Fit::Fit1(double lambda)
{
	unsigned int npar = _par.size();
  for (unsigned int i=0;i<npar;i++)
  {
    _beta[i] = 0.0;
  }
  _alpha.Zero();
  for (unsigned int i=0;i<_x.size();i++)
  {
    double y = _f(_x[i],_par,_deriv);
    double w = 1.0;
    if (_stat) w = (_y[i] <= 0) ? 0 : (1.0 / _y[i]);
    for (unsigned int j=0;j<npar;j++)
    {
      _beta[j] += w * (_y[i] - y) * _deriv[j];
      for (unsigned int k=0;k<npar;k++) _alpha(j,k) += w * _deriv[j] * _deriv[k];
    }
  }
  /* Check for singularities, apply Marquart modification */
  for (unsigned int i=0;i<npar;i++)
  {
    if (_alpha(i,i) < 1.0E-20) _alpha(i,i) = 1.0E-20;
    _alpha(i,i) *= (1.0 + lambda);
  }
  /* Invert the martix */
  _covar = _alpha.Inverse();
  if (_covar.Error())
  {
  	_error = "Matrix inverting failed";
  	return -1.0;
  }
  /* calculate new parameters */
  for (unsigned int i=0;i<npar;i++)
  {
  	if (_par[i]._variable)
  	{
	    for (unsigned int j=0;j<npar;j++)
  	  {
    		_par[i]._value += _beta[j] * _covar(i,j);
	    }
	    if (_par[i]._bounded)
	    {
	    	if (_par[i]._value < _par[i]._lo_bound) _par[i]._value = _par[i]._lo_bound;
	    	if (_par[i]._value > _par[i]._hi_bound) _par[i]._value = _par[i]._hi_bound;
	    }
	  }
  }
  double chi2 = 0;
  for (unsigned int i=0;i<_x.size();i++)
  {
    double y = _f(_x[i],_par,NULL);
    double w = 1.0;
    if (_stat) w = (_y[i] <= 0) ? 0 : (1.0 / _y[i]);
    chi2 += (_y[i] - y) * (_y[i] - y) * w;
  }
  return chi2;
}

/*
 * polynom of arbitrary order (the number of parameters determines the order of the polynom):
 */
double Fit::Polynom(double x, const vector<FitPar> &par, double *d)
{
  double y;

  y = par[par.size()-1]._value;
  for (int i=par.size()-2;i>=0;i--)
  {
    y *= x;
    y += par[i]._value;
  }
  if (d)
  {
    d[0] = 1;
    for (unsigned int i=1;i<par.size();i++)
    {
      d[i] = d[i-1] * x;
    }
  }
  return y;
}

/*
 * exp-function plus background
 */
double Fit::Exponential(double x, const vector<FitPar> &par, double *d)
{
  double y = par[0]._value + par[1]._value * exp(par[2]._value*x);
  if (d)
  {
    d[0] = 1;
    d[1] = exp(par[2]._value*x);
    d[2] = par[1]._value * x * exp(par[2]._value*x);
  }
  return y;
}

/*
 * Gauss-function + background
 */
double Fit::Gaussian(double x, const vector<FitPar> &par, double *d)
{
	double s = par[3]._value;
	if (fabs(s) < 1.0E-38) s = 1.0e-38;
  double arg = (x - par[2]._value) / s;
  double y = par[1]._value / (sqrt(2*M_PI) * s) * exp(-arg*arg/2);
  if (d)
  {
    d[0] = 1;
		if (fabs(par[1]._value) < 1.0E-38)
	  	d[1] = 1 / (sqrt(2*M_PI) * s) * exp(-arg*arg/2);
 		else
	    d[1] = y / par[1]._value;
    d[2] = y * arg / s;
    d[3] = y * (arg * arg  - 1) / s;
  }
  return par[0]._value + y;
}

/*
 * Simple Gauss-function
 */
double Fit::Gaussian1(double x, const vector<FitPar> &par, double *d)
{
	double s = par[2]._value;
	if (fabs(s) < 1.0E-38) s = 1.0e-38;
  double arg = (x - par[1]._value) / s;
  double y = par[0]._value / (sqrt(2*M_PI) * s) * exp(-arg*arg/2);
  if (d)
  {
		if (fabs(par[0]._value) < 1.0E-38)
	  	d[0] = 1 / (sqrt(2*M_PI) * s) * exp(-arg*arg/2);
 		else
	    d[0] = y / par[0]._value;
    d[1] = y * arg / s;
    d[2] = y * (arg * arg  - 1) / s;
  }
  return y;
}


/*
 * sum of gaussians plus background (the number of parameters determines the number of
 * gaussians used):
 */
double Fit::GaussianN(double x, const vector<FitPar> &par, double *d)
{
	double y = par[0]._value;
	if (d) d[0] = 1;
	for (unsigned int i=1;i<par.size();i+=3)
	{
		double s = par[i+2]._value;
		if (fabs(s) < 1.0E-38) s = 1.0e-38;
	  double arg = (x - par[i+1]._value) / s;
	  double yn = par[i]._value / (sqrt(2*M_PI) * s) * exp(-arg*arg/2);
  	y += yn;
	  if (d)
  	{
  		if (fabs(par[i]._value) < 1.0E-38)
		  	d[i] = 1 / (sqrt(2*M_PI) * s) * exp(-arg*arg/2);
  		else
		    d[i] = yn / par[i]._value;
  	  d[i+1] = yn * arg / s;
    	d[i+2] = yn * (arg * arg  - 1) / s;
	  }
	}
	return y;
}

/*
 * sine-function + background
 */
double Fit::Sine(double x, const vector<FitPar> &par, double *d)
{
	double a = par[2]._value * x + par[3]._value;
  double y = par[0]._value + par[1]._value * sin(a);
  if (d)
  {
    d[0] = 1;
  	d[1] = sin(a);
    d[2] = par[1]._value * cos(a) * x;
    d[3] = par[1]._value * cos(a);
  }
  return y;
}

/*
 * cosine-function + background
 */
double Fit::Cosine(double x, const vector<FitPar> &par, double *d)
{
	double a = par[2]._value * x + par[3]._value;
  double y = par[0]._value + par[1]._value * cos(a);
  if (d)
  {
    d[0] = 1;
  	d[1] = cos(a);
    d[2] = -par[1]._value * sin(a) * x;
    d[3] = -par[1]._value * sin(a);
  }
  return y;
}

} /* namespace */

