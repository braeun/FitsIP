#ifndef SIMPLELOWPASSFILTER_H
#define SIMPLELOWPASSFILTER_H

#include "iirfilter.h"

/**
 * @brief A simple low pass filter with a -3dB cutoff frequency.
 */
class SimpleLowPassFilter: public IIRFilter
{
public:
  /**
   * @brief Creates a low pass IIR filter with a given -3dB cutoff frequency
   * expressed as a fraction of the sampling rate.
   * @param fc the cutoff frequency (0 - 0.5)
   */
  SimpleLowPassFilter(ValueType fc);
};

#endif // SIMPLELOWPASSFILTER_H
