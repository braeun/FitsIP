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

#include "Fit2D.h"
#include <iostream>

using namespace std;

#define FIT_LAMBDAINI 1.e-3   /* Initial value for lambda */
#define FIT_LAMBDAMIN 0.      /* Min lambda we need for a successful fit */
#define FIT_LAMBDAMAX 1.e+5   /* Max lambda we use to stop divergence */
#define FIT_ITERMAX   100     /* Max itarations we allow */
#define FIT_CHIEND1   1.e-5   /* Chi-square end criterium of fit */
#define FIT_CHIEND2   1.e-4   /* Chi-square end criterium of fit */


namespace HMath {


const char *Fit2D::_description[] = {
			"Gaussian with Background",
};


const char *Fit2D::_equation[] = {
			"z = a0 + a1/(4pi*a3*a5)*exp(-((x-a2)/(2*a3))^2-((y-a4)/(2*a5))^2)",
};

Fit2D::Fit2D(void)
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

Fit2D::~Fit2D(void)
{
	if (_deriv) delete [] _deriv;
	if (_beta) delete [] _beta;
}

/*
 * Return the number of internally defined fit functions (static method)
 */
int Fit2D::GetNrInternalFunctions(void)
{
	return FIT2D_MAXFUNCTION;
}

/*
 * Return the equation string for the internally defined fit function with index @a i (static method)
 */
const char *Fit2D::FunctionString(int i)
{
	if (i >= 0 && i < FIT2D_MAXFUNCTION) return _equation[i];
	return "unknown";
}

/*
 * Return the description for the internally defined fit function with index @a i (static method)
 */
const char *Fit2D::GetFunctionDescription(int i)
{
	if (i >= 0 && i < FIT2D_MAXFUNCTION) return _description[i];
	return "unknown";
}

/*
 * Set an internal fit function
 */
bool Fit2D::SetFunction(int f)
{
	switch (f)
	{
		case FIT2D_GAUSSIAN:
			_f = Gaussian2D;
			_minpar = 6;
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
bool Fit2D::SetFunction(int id, double (*f)(double, double, const vector<FitPar>&, double*), int npar,  const char *txt)
{
	_f = f;
	_fid = id;
	_minpar = npar;
	_fname = txt;
	return true;
}

/*
 * Calculate z value at @x and @a y for fitted function
 */
double Fit2D::CalculateZ(double x, double y)
{
	if (_f) return _f(x,y,_par,NULL);
	return 0;
}

/*
 * Perform the fit and return the reduced chi2 value. If an error occured, a chi2 < 0 is returned.
 */
double Fit2D::Execute(void)
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
    double z = _f(_x[i],_y[i],_par,NULL);
    double w = 1.0;
    if (_stat) w = (_z[i] <= 0) ? 0 : (1.0 / _z[i]);
    oldchi2 += (_z[i] - z) * (_z[i] - z) * w;
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

double Fit2D::Fit1(double lambda)
{
	unsigned int npar = _par.size();
  for (unsigned int i=0;i<npar;i++)
  {
    _beta[i] = 0.0;
  }
  _alpha.Zero();
  for (unsigned int i=0;i<_x.size();i++)
  {
    double z = _f(_x[i],_y[i],_par,_deriv);
    double w = 1.0;
    if (_stat) w = (_z[i] <= 0) ? 0 : (1.0 / _z[i]);
    for (unsigned int j=0;j<npar;j++)
    {
      _beta[j] += w * (_z[i] - z) * _deriv[j];
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
    double z = _f(_x[i],_y[i],_par,NULL);
    double w = 1.0;
    if (_stat) w = (_z[i] <= 0) ? 0 : (1.0 / _z[i]);
    chi2 += (_z[i] - z) * (_z[i] - z) * w;
  }
  return chi2;
}

/*
 * 2 dimensional Gauss-function + background
 */
double Fit2D::Gaussian2D(double x, double y, const vector<FitPar> &par, double *d)
{
  double argx = (x - par[2]._value) / par[3]._value;
  double argy = (y - par[4]._value) / par[5]._value;
  double argexp = argx * argx / 2 + argy * argy / 2;
  double z = par[1]._value / (2 * M_PI * par[3]._value * par[5]._value) * exp(-argexp);
  if (d)
  {
    d[0] = 1;
		if (fabs(par[1]._value) < 1.0E-38)
	  	d[1] = 1 / (4 * M_PI *par[3]._value * par[5]._value) * exp(-argexp);
 		else
	    d[1] = z / par[1]._value;
	  d[2] = z * argx / par[3]._value;
	  d[3] = z * (argx * argx  - 1) / par[3]._value;
	  d[4] = z * argy / par[5]._value;
	  d[5] = z * (argy * argy  - 1) / par[5]._value;
  }
  return z + par[0]._value;
}

} /* namespace */
