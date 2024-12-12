#ifndef IIRFILTER_H
#define IIRFILTER_H

#include "filter.h"
#include <vector>

class IIRFilter: public Filter
{
public:
  IIRFilter();
  virtual ~IIRFilter();

  /**
   * @brief Get the direction of the filter.
   * @return the direction
   */
  Direction getDirection() const;

  /**
   * @brief Return the filter coefficients 'a'
   * @return filter coefficients 'a'
   */
  const std::vector<ValueType>& getA() const;

  /**
   * @brief Return the filter coefficients 'b'
   * @return filter coefficients 'b'
   */
  const std::vector<ValueType>& getB() const;

  /**
   * @brief Normalizes the gain.
   *
   * The gain can be normalized at a frequency of 0 (used for low pass filters)
   * or at a frequency of 0.5 (used for high pass filters).
   * @param center if true normalize at a frequency of 0.5, otherwise normalize at 0
   */
  void normalizeGain(bool center);

  virtual ValueType* apply(ValueType* data, uint32_t size) override;

  virtual ValueType* apply(ValueType* data, uint32_t width, uint32_t height) override;

protected:
  std::vector<ValueType> a;
  std::vector<ValueType> b;
  Direction dir;

private:
  ValueType* applyForward(ValueType* data, uint32_t size);
  ValueType* applyForward(ValueType* data, uint32_t width, uint32_t height);
  ValueType* applyBackward(ValueType* data, uint32_t size);
  ValueType* applyBackward(ValueType* data, uint32_t width, uint32_t height);


};

#endif // IIRFILTER_H
