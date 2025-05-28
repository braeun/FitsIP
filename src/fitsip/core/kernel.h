/********************************************************************************
 *                                                                              *
 * FitsIP - definition of a filter kernel                                       *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#ifndef KERNEL_H
#define KERNEL_H

#include "fitstypes.h"
#include <QString>
#include <vector>

class Kernel
{
public:
  Kernel();
  Kernel(const QString& n, const QString& type, uint32_t w, uint32_t h);
  Kernel(const QString& n, const QString& type, uint32_t w, uint32_t h, const std::vector<std::vector<ValueType>>& k);

  const QString& getName() const;

  const QString& getType() const;

  bool isEmpty() const;

  uint32_t getWidth() const;

  uint32_t getHeight() const;

  void normalize();

  const std::vector<std::vector<ValueType>>& getKernel() const;

  /**
   * @brief Return the vector of columns in a given row.
   * @param row the row index
   * @return the vector of columns
   */
  inline const std::vector<ValueType>& operator[](uint32_t row) const;

  /**
   * @brief Return the vector of columns in a given row.
   * @param row the row index
   * @return the vector of columns
   */
  inline std::vector<ValueType>& operator[](uint32_t row);

private:
  QString name;
  QString type;
  uint32_t width;
  uint32_t height;
  std::vector<std::vector<ValueType>> kern;
};

const std::vector<ValueType>& Kernel::operator[](uint32_t index) const
{
  return kern[index];
}

std::vector<ValueType>& Kernel::operator[](uint32_t index)
{
  return kern[index];
}



#endif // KERNEL_H
