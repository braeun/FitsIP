/********************************************************************************
 *                                                                              *
 * FitsIP - types and simple structs used                                       *
 *                                                                              *
 * modified: 2025-01-04                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of FitsIP.                                                 *
 * FitsIP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * FitsIP is distributed in the hope that it will be useful, but                *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * FitsIP. If not, see <https://www.gnu.org/licenses/>.                         *
 ********************************************************************************/

#ifndef FITSTYPES_H
#define FITSTYPES_H

#include <tuple>
#include <cstdint>

#ifdef USE_FLOAT
using ValueType = float;
#else
using ValueType = double;
#endif

struct AverageResult
{
  /** @brief number of values */
  int64_t n;
  /** @brief mean value */
  double mean;
  /** @brief width of the distribution */
  double sigma;
  /** @brief error ov the mean value */
  double error;
};

#endif // CONFIG_H
