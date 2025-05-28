#ifndef CHEBYSHEVFILTER_H
#define CHEBYSHEVFILTER_H


#include "iirfilter.h"

/**
 * @brief A Chebyshev filter.
 */
class ChebyshevFilter: public IIRFilter
{
public:
  /**
   * @brief Creates a Chebyshev filter.
   * @param fc cut off frequency
   * @param pr ripple (%, 0-29)
   * @param np number of poles (2-20)
   * @param lh true for high pass filter, false for low pass
   */
  ChebyshevFilter(ValueType fc, ValueType pr, int np, bool lh);

private:
  void s(ValueType fc, ValueType pr, int np, int p, bool lh);

  ValueType a0,a1,a2;
  ValueType b1,b2;


};

#endif // CHEBYSHEVFILTER_H
