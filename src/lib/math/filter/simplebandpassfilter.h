#ifndef SIMPLEBANDPASSFILTER_H
#define SIMPLEBANDPASSFILTER_H

#include "iirfilter.h"

/**
 * @brief A simple band pass filter.
 */
class SimpleBandPassFilter: public IIRFilter
{
public:
  /**
   * @brief Creates a band pass IIR filter for a given center frequency and
   * band width expressed as a fraction of the sampling rate.
   * @param f the center frequency (0 - 0.5)
   * @param bw the band width (0 - 0.5)
   */
  SimpleBandPassFilter(ValueType f, ValueType bw);
};

#endif // SIMPLEBANDPASSFILTER_H
