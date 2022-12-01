#include "chebyshevfilter.h"
#include <cmath>
#include <cstring>

ChebyshevFilter::ChebyshevFilter(ValueType fc, ValueType pr, int np, bool lh)
{
  std::vector<ValueType> da(23);
  std::vector<ValueType> db(23);
  memset(da.data(),0,da.size()*sizeof(ValueType));
  memset(db.data(),0,da.size()*sizeof(ValueType));
  da[2] = 1;
  db[2] = 1;
  if (np < 2) np = 2;
  for (int p=1;p<=np/2;p++)
  {
    s(fc,pr,np,p,lh);
    std::vector<ValueType> ta = da;
    std::vector<ValueType> tb = db;
    for (uint32_t i=2;i<23;i++)
    {
      da[i] = a0 * ta[i] + a1 * ta[i-1] + a2 * ta[i-2];
      db[i] = tb[i] - b1 * tb[i-1] - b2 * tb[i-2];
    }
  }
  /* finish combining coefficients */
  a = std::vector<ValueType>(21);
  b = std::vector<ValueType>(21);
  for (int i=0;i<21;i++)
  {
    a[i] = da[i+2];
    b[i] = -db[i+2];
  }
  b[0] = 0;
  /* normalize the gain */
  ValueType sa = 0;
  ValueType sb = 0;
  for (uint32_t i=0;i<21;i++)
  {
    if (lh)
    {
      sa += a[i] * ((i % 2) == 0 ? 1 : -1);
      sb += b[i] * ((i % 2) == 0 ? 1 : -1);
    }
    else
    {
      sa += a[i];
      sb += b[i];
    }
  }
  ValueType gain = sa / (1 - sb);
  for (uint32_t i=0;i<21;i++)
  {
    a[i] /= gain;
  }
}

void ChebyshevFilter::s(ValueType fc, ValueType pr, int np, int p, bool lh)
{
  /* Calculate the pole location on the unit circle */
  ValueType rp = -cos(M_PI/(np*2)+(p-1)*M_PI/np);
  ValueType ip = sin(M_PI/(np*2)+(p-1)*M_PI/np);
  /* Warp from a circle to an ellipse */
  if (pr > 0)
  {
    ValueType pr1 = 100.0 / (100.0 - pr);
    ValueType es = sqrt(pr1*pr1-1);
    ValueType vx = (1.0 / np) * log((1.0/es) + sqrt((1.0/es/es)+1));
    ValueType kx = (1.0 / np) * log((1.0/es) + sqrt((1.0/es/es)-1));
    kx = (exp(kx) + exp(-kx)) / 2.0;
    rp *= ((exp(vx) - exp(-vx)) / 2.0) / kx;
    ip *= ((exp(vx) + exp(-vx)) / 2.0) / kx;
  }
  /* s-domain to z-domain conversion */
  ValueType t = 2.0 * tan(0.5);
  ValueType w = 2.0 * M_PI * fc;
  ValueType m = rp * rp + ip * ip;
  ValueType d = 4.0 - 4.0 * rp * t + m * t * t;
  ValueType x0 = t * t / d;
  ValueType x1 = 2 * t * t / d;
  ValueType x2 = t * t / d;
  ValueType y1 = (8 - 2 * m * t * t) / d;
  ValueType y2 = (-4 - 4 * rp * t - m * t * t) / d;
  /* LP TO LP, or LP TO HP transform */
  ValueType k;
  if (lh)
    k = -cos(w/2+0.5) / cos(w/2-0.5);
  else
    k = sin(0.5-w/2) / sin(0.5+w/2);
  d = 1 + y1 * k - y2 * k * k;
  a0 = (x0 - x1 * k + x2 * k * k) / d;
  a1 = (-2 * x0 * k + x1 + x1 * k * k - 2 * x2 * k) / d;
  a2 = (x0 * k * k - x1 * k + x2) / d;
  b1 = (2 * k + y1 + y1 * k * k - 2 * y2 * k) / d;
  b2 = (-k * k - y1 * k + y2) / d;
  if (lh)
  {
    a1 *= -1;
    b1 *= -1;
  }
}
