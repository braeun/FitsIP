#ifndef PIXEL_H
#define PIXEL_H

#include "fitstypes.h"
#include <cstdint>
#include <vector>

/**
 * @brief Utility class representing the data of a single pixel.
 */
class Pixel
{
public:
  uint32_t x = 0;   /*!< x position */
  uint32_t y = 0;   /*!< y position */
  ValueType v = 0;  /*!< integral value */
  std::vector<ValueType> i; /*!< layer values */
};


#endif // PIXEL_H
