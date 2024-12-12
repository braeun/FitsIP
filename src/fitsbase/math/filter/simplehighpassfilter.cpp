#include "simplehighpassfilter.h"
#include <cmath>

SimpleHighPassFilter::SimpleHighPassFilter(ValueType fc)
{
  ValueType x = exp(-2*M_PI*fc);
  a.push_back((1+x)/2);
  a.push_back(-(1+x)/2);
  b.push_back(0);
  b.push_back(x);
}
