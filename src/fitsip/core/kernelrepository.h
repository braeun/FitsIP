/********************************************************************************
 *                                                                              *
 * FitsIP - kernel repository for filters                                       *
 *                                                                              *
 * modified: 2025-12-29                                                         *
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

#ifndef KERNELREPOSITORY_H
#define KERNELREPOSITORY_H

#include "kernel.h"
#include <map>
#include <vector>

class KernelRepository
{
public:
  KernelRepository();

  std::vector<QString> getPredefinedKernelNames() const;

  const Kernel& getKernel(const QString& name) const;

  /**
   * @brief Create a Gaussian kernel with a given sigma and accuracy.
   * @param sigma the sigma in pixels
   * @param accuracy the accuracy in percent of the peak value
   * @return the kernel
   */
  Kernel createGaussianKernel(double sigma, double accuracy=0.01) const;

  /**
   * @brief Create a Laplacian of Gaussian kernel with a given sigma and accuracy.
   * @param sigma the sigma of the Gaussian in pixels
   * @param accuracy the accuracy in percent of the peak value of the Gaussian function
   * @return the kernel
   */
  Kernel createLoGKernel(double sigma, double accuracy=0.01) const;

  static KernelRepository& instance();

  static const char* BOXCAR3;
  static const char* BOXCAR5;
  static const char* BLUR50;
  static const char* GENTLEBLUR;
  static const char* MINIMALBLUR;
  static const char* GAUSSIAN3;
  static const char* GAUSSIAN5;
  static const char* DONUT;
  static const char* TRIANGLE5;
  static const char* SHARPEN;
  static const char* SHARPEN_GAUSSIAN;
  static const char* CRISPEN1;
  static const char* CRISPEN2;
  static const char* SOBEL_X;
  static const char* SOBEL_Y;
  static const char* LAPLACIAN;
  static const char* LAPLACIAN2;

private:
  std::map<QString,Kernel> kernels;

  static KernelRepository globalRepository;
};

#endif // KERNELREPOSITORY_H
