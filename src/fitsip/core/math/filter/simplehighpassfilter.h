#ifndef SIMPLEHIGHPASSFILTER_H
#define SIMPLEHIGHPASSFILTER_H

#include "iirfilter.h"

/**
 * @brief A simple high pass filter with a -3dB cutoff frequency.
 */
class SimpleHighPassFilter: public IIRFilter
{
public:
  /**
   * @brief Creates a high pass IIR filter with a given -3dB cutoff frequency
   * expressed as a fraction of the sampling rate.
   * @param fc the cutoff frequency (0 - 0.5)
   */
  SimpleHighPassFilter(ValueType fc);
};

#endif // SIMPLEHIGHPASSFILTER_H
