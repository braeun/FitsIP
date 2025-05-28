#include "simplebandpassfilter.h"
#include <cmath>

SimpleBandPassFilter::SimpleBandPassFilter(ValueType f, ValueType bw)
{
  ValueType r = 1 - 3 * bw;
  ValueType k = (1 - 2 * r * cos(2*M_PI*f) + r * r) / (2 - 2 * cos(2*M_PI*f));
  a = std::vector<ValueType>(3);
  a[0] = k;
  a[1] = -2 * k * cos(2*M_PI*f);
  a[2] = -k;
  b = std::vector<ValueType>(3);
  b[0] = 0;
  b[1] = 2 * r * cos(2*M_PI*f);
  b[2] = -r * r;
}
