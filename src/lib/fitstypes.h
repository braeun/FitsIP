#ifndef FITSTYPES_H
#define FITSTYPES_H

#include <tuple>

#ifdef USE_FLOAT
using ValueType = float;
#else
using ValueType = double;
#endif

/**
 * @brief The AverageResult tuple consists of n (int), mean (double), sigma (double) and error (double)
 *
 * Here sigma reflects the width of the distribution and error is the
 * uncertainty of the mean.
 */
using AverageResult = std::tuple<int,double,double,double>;

#endif // CONFIG_H
