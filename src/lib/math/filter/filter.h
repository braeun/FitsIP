#ifndef FILTER_H
#define FILTER_H

#include "../../fitstypes.h"
#include <memory>

class Layer;

class Filter
{
public:

  enum Direction { FORWARD, BACKWARD, SYMMETRIC };

  virtual ValueType* apply(ValueType* data, uint32_t size) = 0;

  virtual ValueType* apply(ValueType* data, uint32_t width, uint32_t height) = 0;

protected:
  Filter();
  virtual ~Filter();
};

#endif // FILTER_H
