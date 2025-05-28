#include "utils.h"

namespace math {

/* given two numbers 'larger' and 'smaller', return the fraction of
   the way from 'larger' to 'smaller' that you have to go to get
   to the number 'middle'.

   i.e.  larger=10, smaller=4, middle=6.0 --> return 0.66

 */
double get_frac(double larger, double smaller, double middle)
{
  double a = larger - smaller;
  double b = larger - middle;
  if (a <= b || a <= 0.0)	return(0.0);
  return b / a;
}


} // namespace math
