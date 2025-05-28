#include "simplelowpassfilter.h"
#include <cmath>

SimpleLowPassFilter::SimpleLowPassFilter(ValueType fc)
{
  ValueType x = exp(-2*M_PI*fc);
  a.push_back(1-x);
  a.push_back(0);
  b.push_back(0);
  b.push_back(x);
}
