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

#include "Math.h"
#include <iostream>

using namespace std;

#define ITMAX			100
#define EPS				3.0E-7
#define FPMIN			1.0E-30

namespace HMath {

/*
 * Order the double values
 */
void OrderMinMax(double &min, double &max)
{
  if (max < min)
  {
    double tmp = max;
    max = min;
    min = tmp;
  }
}

/*
 * Order the integer values
 */
void OrderMinMax(int &min, int &max)
{
  if (max < min)
  {
    int tmp = max;
    max = min;
    min = tmp;
  }
}

/*-----------------------------------------------------------------------------------*/
/*                                   functions                                       */
/*-----------------------------------------------------------------------------------*/

/*
 * Polynome of order n - 1
 */
double Polynom(double x, int n, double par[], double d[])
{
  double y;

  y = par[n-1];
  for (int i=n-2;i>=0;i--)
  {
    y *= x;
    y += par[i];
  }
  if (d)
  {
    d[0] = 1;
    for (int i=1;i<n;i++)
    {
      d[i] = d[i-1] * x;
    }
  }
  return y;
}

/*
 * exp-function plus background
 */
double Exponential(double x, int n, double par[], double d[])
{
  double y = par[0] + par[1] * exp(par[2]*x);
  if (d)
  {
    d[0] = 1;
    d[1] = exp(par[2]*x);
    d[2] = par[1] * x * exp(par[2]*x);
  }
  return y;
}

/*
 *	Gauss-function + background
 */
double Gaussian(double x, int n, double par[], double d[])
{
	if (fabs(par[3]) < 1.0E-38) par[3] = 1.0e-38;
  double arg = (x - par[2]) / par[3];
  double y = par[1] / (sqrt(2*M_PI) * par[3]) * exp(-arg*arg/2);
  if (d)
  {
    d[0] = 1;
		if (fabs(par[1]) < 1.0E-38)
	  	d[1] = 1 / (sqrt(2*M_PI) * par[3]) * exp(-arg*arg/2);
 		else
	    d[1] = y / par[1];
    d[2] = y * arg / par[3];
    d[3] = -y * (1 / par[3] - 2 * arg * arg / par[3]);;
  }
  return par[0] + y;
}

/*
 * Simple Gauss-function
 */
double Gaussian1(double x, int n, double par[], double d[])
{
	if (fabs(par[2]) < 1.0E-38) par[2] = 1.0e-38;
  double arg = (x - par[1]) / par[2];
  double y = par[0] / (sqrt(2*M_PI) * par[2]) * exp(-arg*arg/2);
  if (d)
  {
		if (fabs(par[0]) < 1.0E-38)
	  	d[0] = 1 / (sqrt(2*M_PI) * par[2]) * exp(-arg*arg/2);
 		else
	    d[0] = y / par[0];
    d[1] = y * arg / par[2];
    d[2] = -y * (1 / par[2] - 2 * arg * arg / par[2]);;
  }
  return y;
}


/*
 * 2 dimensional Gauss-function + background
 */
double Gaussian2D(double x, double y, int n, double par[], double d[])
{
  double argx = (x - par[2]) / par[3];
  double argy = (y - par[4]) / par[5];
  double argexp = argx * argx + argy * argy;
  double z = par[0] + par[1] * exp(-argexp);
  if (d)
  {
    d[0] = 1;
    d[1] = exp(-argexp);
    d[2] = par[1] * exp(-argexp) * 2 * argx / par[3];
    d[3] = par[1] * exp(-argexp) * 2 * argx * argx / par[3];
    d[4] = par[1] * exp(-argexp) * 2 * argy / par[5];
    d[5] = par[1] * exp(-argexp) * 2 * argy * argy / par[5];
  }
  return z;
}

/*
 * Sum of Gauss-functions + constant background
 */
double GaussianN(double x, int n, double par[], double d[])
{
	double y = par[0];
	if (d) d[0] = 1;
	for (int i=1;i<n;i+=3)
	{
	  double arg = (x - par[i+1]) / par[i+2];
	  double yn = par[i] / (sqrt(2*M_PI) * par[i+2]) * exp(-arg*arg/2);
  	y += yn;
	  if (d)
  	{
  		if (fabs(par[i]) < 1.0E-38)
		  	d[i] = 1 / (sqrt(2*M_PI) * par[i+2]) * exp(-arg*arg/2);
  		else
		    d[i] = yn / par[i];
  	  d[i+1] = yn * arg / par[i+2];
    	d[i+2] = -yn * (1 / par[i+2] - 2 * arg * arg / par[i+2]);;
	  }
	}
	return y;
}


/*
 * Compute ln(Gamma(x))
 * Algorithm based on Press et al. Numerical Recipes in C
 */
double GammaLn(double x)
{
	static double cof[] = { 76.18009172947146, -86.50532032941677,
                          24.01409824083091,  -1.231739572450155,
                           0.1208650973866E-2, -0.5395239384953E-5 };

	double y = x;
	double tmp = x + 5.5;
	tmp -= (x + 0.5) * log(tmp);
	double ser = 1.000000000190015;
	for (int j=0;j<6;j++) ser += cof[j] / ++y;
	return -tmp + log(2.5066282746310005*ser/x);
}

/*
 * Compute incomplete gamma function P(a,x) evaluated by its series representation
 * Algorithm based on Press et al. Numerical Recipes in C
 */
double gser(double a, double x, double *gln)
{
	*gln = GammaLn(a);
	if (x <= 0)
	{
		cout << "(E) x less than 0 in function gser" << endl;
		return 0;
	}
	double ap = a;
	double del = 1.0 / a;
	double sum = del;
	for (int n=0;n<ITMAX;n++)
	{
		ap++;
		del *= x / ap;
		sum += del;
		if (fabs(del) < fabs(sum)*EPS) return sum * exp(-x+a*log(x)-(*gln));
	}
	cout << "(W) a too large, ITMAX too small in function gser" << endl;
	return 0;
}

/*
 * Compute incomplete gamma function Q(a,x) evaluated by its continued fraction representation
 * Algorithm based on Press et al. Numerical Recipes in C
 */
double gcf(double a, double x, double *gln)
{
	*gln = GammaLn(a);
	double b = x + 1.0 - a;
	double c = 1.0 / FPMIN;
	double d = 1.0 / b;
	double h = d;
	for (int i=0;i<ITMAX;i++)
	{
		double an = -i * (i - a);
		b += 2;
		d = d * an + b;
		if (fabs(d) < FPMIN) d = FPMIN;
		c = b + an / c;
		if (fabs(c) < FPMIN) c = FPMIN;
		d = 1.0 / d;
		double del = d * c;
		h *= del;
		if (fabs(del-1.0) < EPS) return exp(-x+a*log(x)-(*gln)) * h;
	}
	cout << "(W) a too large, ITMAX too small in function gcf" << endl;
	return 0;
}


/*
 * Compute incomplete gamma function P(a,x)
 * Algorithm based on Press et al. Numerical Recipes in C
 */
double GammaP(double a, double x)
{
	double gln;

	if (x < 0.0 || a <= 0.0)
	{
		cout << "(E) Invalid arguments in function GammaP" << endl;
		return 0;
	}
	if (x < (a+1)) return gser(a,x,&gln);
	return 1.0 - gcf(a,x,&gln);
}

/*
 * Compute incomplete gamma function Q(a,x) = 1 - P(a,x)
 * Algorithm based on Press et al. Numerical Recipes in C
 */
double GammaQ(double a, double x)
{
	double gln;

	if (x < 0.0 || a <= 0.0)
	{
		cout << "(E) Invalid arguments in function GammaP" << endl;
		return 0;
	}
	if (x < (a+1)) return 1.0 - gser(a,x,&gln);
	return gcf(a,x,&gln);
}


/*
 * Fit y = a + b * x to given data in vectors x and y. Return chi square as well as the errors
 * of a and b if the respective pointers are not NULL.
 * This fit is based on the formulas given by L. Lyons: A Practical Guide to Data Analysis for
 * Physical Science Students.
 */
double LinFit(const vector<double> &x, const vector<double> &y, double *a, double *b, double *erra, double *errb)
{
	if (x.size() != y.size() || x.size() < 2) return -1.0;
	unsigned int n = x.size();
	double sx = 0;
	double sy = 0;
	double sxy = 0;
	double sx2 = 0;
	for (unsigned int i=0;i<n;i++)
	{
		sx += x[i];
		sy += y[i];
		sxy += x[i] * y[i];
		sx2 += x[i] * x[i];
	}
	double xmean = sx / n;
	double ymean = sy / n;
	double denom = n * sx2 - sx * sx;
	if (fabs(denom) < 1.0E-15) return -1;
	*b = (n * sxy - sx * sy) / denom;
	*a = ymean - xmean * (*b);
	double chi2 = 0;
	for (unsigned int i=0;i<n;i++)
	{
		double d = y[i] - (*a + (*b) * x[i]);
		chi2 += d * d;
	}
	if (errb)
	{
		if (n < 3)
		{
			*errb = -1;
			if (erra) *erra = -1;
		}
		else
		{
			double sigma2 = chi2 / (n - 2);
			double sx2p = sx2 - sx * sx / n;
			*errb = sqrt(sigma2/sx2p);
			if (erra)
			{
				*erra = sqrt(sigma2/n+(xmean*(*errb))*(xmean*(*errb)));
			}
		}
	}
	return chi2;
}

/*
 * Fit y = a + b * x to given data in vectors x and y using the standard deviations sigma.
 * Return chi square as well as the errors
 * of a and b if the respective pointers are not NULL.
 * This fit is based on the formulas given by L. Lyons: A Practical Guide to Data Analysis for
 * Physical Science Students.
 */
double WeightedLinFit(const vector<double> &x, const vector<double> &y, const vector<double> &sigma, double *a, double *b, double *erra, double *errb)
{
	if (x.size() != y.size() || x.size() < 2) return -1.0;
	double n = 0;
	double sx = 0;
	double sy = 0;
	double sxy = 0;
	double sx2 = 0;
	for (unsigned int i=0;i<x.size();i++)
	{
		if (fabs(sigma[i]) < 1.0E-15) continue;
		n += 1.0 / (sigma[i] * sigma[i]);
		sx += x[i] / (sigma[i] * sigma[i]);
		sy += y[i] / (sigma[i] * sigma[i]);
		sxy += x[i] * y[i] / (sigma[i] * sigma[i]);
		sx2 += x[i] * x[i] / (sigma[i] * sigma[i]);
	}
	double xmean = sx / n;
	double ymean = sy / n;
	double denom = n * sx2 - sx * sx;
	if (fabs(denom) < 1.0E-15) return -1;
	*b = (n * sxy - sx * sy) / denom;
	*a = ymean - xmean * (*b);
	double chi2 = 0;
	for (unsigned int i=0;i<x.size();i++)
	{
		double d = (y[i] - (*a + (*b) * x[i])) / sigma[i];
		chi2 += d * d;
	}
	if (errb)
	{
		double sx2p = sx2 - sx * sx / n;
		*errb = sqrt(1.0/sx2p);
		if (erra)
		{
			*erra = sqrt(1.0/n+xmean*xmean/sx2p);
		}
	}
	return chi2;
}

} /* namespace */
