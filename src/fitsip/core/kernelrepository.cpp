/********************************************************************************
 *                                                                              *
 * FitsIP - kernel repository for filters                                       *
 *                                                                              *
 * modified: 2022-11-21                                                         *
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

#include "kernelrepository.h"
#include <algorithm>
#include <cmath>

static const Kernel EMPTY;

KernelRepository KernelRepository::globalRepository;

const char* KernelRepository::BOXCAR3 = "Boxcar (3x3)";
const char* KernelRepository::BOXCAR5 = "Boxcar (5x5)";
const char* KernelRepository::BLUR50 = "50% Blur (3x3)";
const char* KernelRepository::GENTLEBLUR = "Gentle Blur (3x3)";
const char* KernelRepository::MINIMALBLUR = "Minimal Blur (3x3)";
const char* KernelRepository::GAUSSIAN3 = "Gaussian Blur (3x3)";
const char* KernelRepository::GAUSSIAN5 = "Gaussian Blur (5x5)";
const char* KernelRepository::DONUT = "Donut (3x3)";
const char* KernelRepository::TRIANGLE5 = "Triangle Blur (5x5)";
const char* KernelRepository::SHARPEN = "Sharpen (3x3)";
const char* KernelRepository::SHARPEN_GAUSSIAN = "Sharpen Gaussian (5x5)";
const char* KernelRepository::CRISPEN1 = "Crispen 1 (3x3)";
const char* KernelRepository::CRISPEN2 = "Crispen 2 (3x3)";
const char* KernelRepository::SOBEL_X = "Sobel X (3x3)";
const char* KernelRepository::SOBEL_Y = "Sobel Y (3x3)";
const char* KernelRepository::LAPLACIAN = "Laplacian";
const char* KernelRepository::LAPLACIAN2 = "Laplacian2";

KernelRepository::KernelRepository()
{
  /*
   * The following kernels are from R.Berry and J.Burnell: Handbook of Astronomical
   * Image Processing
   */
  kernels.insert(std::make_pair(BOXCAR3,Kernel(BOXCAR3,"Generic",3,3,{{1,1,1},{1,1,1},{1,1,1}})));
  kernels.insert(std::make_pair(BOXCAR5,Kernel(BOXCAR5,"Generic",5,5,{{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}})));
  kernels.insert(std::make_pair(BLUR50,Kernel(BLUR50,"Generic",3,3,{{1,1,1},{1,2,1},{1,1,1}})));
  kernels.insert(std::make_pair(GENTLEBLUR,Kernel(GENTLEBLUR,"Generic",3,3,{{1,1,1},{1,8,1},{1,1,1}})));
  kernels.insert(std::make_pair(MINIMALBLUR,Kernel(MINIMALBLUR,"Generic",3,3,{{0,1,0},{1,8,1},{0,1,0}})));
  kernels.insert(std::make_pair(GAUSSIAN3,Kernel(GAUSSIAN3,"Generic",3,3,{{1,2,1},{2,4,2},{1,2,1}})));
  kernels.insert(std::make_pair(GAUSSIAN5,Kernel(GAUSSIAN5,"Generic",5,5,{{1,4,6,4,1},{4,16,24,16,4},{6,24,36,24,6},{4,16,24,16,4},{1,4,6,4,1}})));
  kernels.insert(std::make_pair(DONUT,Kernel(DONUT,"Generic",3,3,{{1,1,1},{1,0,1},{1,1,1}})));
  kernels.insert(std::make_pair(TRIANGLE5,Kernel(TRIANGLE5,"Generic",5,5,{{0,0,1,0,0},{0,1,2,1,0},{1,2,3,2,1},{0,1,2,1,0},{0,0,1,0,0}})));
  kernels.insert(std::make_pair(SHARPEN,Kernel(SHARPEN,"Generic",3,3,{{-1,-1,-1},{-1,9,-1},{-1,-1,-1}})));
  kernels.insert(std::make_pair(SHARPEN_GAUSSIAN,Kernel(SHARPEN_GAUSSIAN,"Generic",5,5,{{-1,-4,-6,-4,-1},{-4,-16,-24,-16,-4},{-6,-24,220,-24,-6},{-4,-16,-24,-16,-4},{-1,-4,-6,-4,-1}})));
  kernels.insert(std::make_pair(CRISPEN1,Kernel(CRISPEN1,"Generic",3,3,{{0,-1,0},{-1,5,-1},{0,-1,0}})));
  kernels.insert(std::make_pair(CRISPEN2,Kernel(CRISPEN2,"Generic",3,3,{{1,-2,1},{-2,5,-2},{1,-2,1}})));
  kernels.insert(std::make_pair(SOBEL_X,Kernel(SOBEL_X,"Generic",3,3,{{-1,0,1},{-2,0,2},{-1,0,1}})));
  kernels.insert(std::make_pair(SOBEL_Y,Kernel(SOBEL_Y,"Generic",3,3,{{-1,-2,-1},{0,0,0},{1,2,1}})));

  kernels.insert(std::make_pair(LAPLACIAN,Kernel(LAPLACIAN,"Generic",3,3,{{0,-1,0},{-1,4,-1},{0,-1,0}})));
  kernels.insert(std::make_pair(LAPLACIAN2,Kernel(LAPLACIAN2,"Generic",3,3,{{-1,-1,-1},{-1,8,-1},{-1,-1,-1}})));
}

std::vector<QString> KernelRepository::getPredefinedKernelNames() const
{
  std::vector<QString> list;
  for (const auto& entry : kernels) list.push_back(entry.first);
  std::sort(list.begin(),list.end());
  return list;
}

const Kernel& KernelRepository::getKernel(const QString &name) const
{
  if (kernels.find(name) == kernels.end()) return EMPTY;
  return kernels.at(name);
}

Kernel KernelRepository::createGaussianKernel(double sigma, double accuracy) const
{
  if (sigma <= 0) throw std::invalid_argument("sigma must be > 0");
  if (accuracy <= 0) throw std::invalid_argument("accuracy must be > 0");
  int size = 2 * static_cast<int>(ceil(sigma * sqrt(-2 * ::log(accuracy)))) + 1;
  int center = size / 2;
  double s2 = 2 * sigma * sigma;
  double norm = 1.0 / (M_PI * s2);
  std::vector<std::vector<ValueType>> data;
  for (int j=0;j<size;++j)
  {
    double j2 = (j - center) * (j - center);
    for (int i=0;i<size;++i)
    {
      double i2 = (i - center) * (i - center);
      data[j][i] = norm * ::exp((i2+j2)/s2);
    }
  }
  return Kernel(QString::asprintf("Gaussian %.2f sigma",sigma),"gaussian",size,size,data);
}

Kernel KernelRepository::createLoGKernel(double sigma, double accuracy) const
{
  if (sigma <= 0) throw std::invalid_argument("sigma must be > 0");
  if (accuracy <= 0) throw std::invalid_argument("accuracy must be > 0");
  int size = 2 * static_cast<int>(ceil(sigma * sqrt(-2 * ::log(accuracy)))) + 1;
  /* empirically increase size for the laplacian */
  size += 2;
  int center = size / 2;
  double s2 = 2 * sigma * sigma;
  double norm = -1.0 / (M_PI * sigma * sigma * sigma * sigma);
  std::vector<std::vector<ValueType>> data;
  for (int j=0;j<size;++j)
  {
    double j2 = (j - center) * (j - center);
    for (int i=0;i<size;++i)
    {
      double i2 = (i - center) * (i - center);
      data[j][i] = norm * (1 - (i2+j2)/s2) * ::exp((i2+j2)/s2);
    }
  }
  return Kernel(QString::asprintf("Gaussian %.2f sigma",sigma),"gaussian",size,size,data);
}

KernelRepository& KernelRepository::instance()
{
  return globalRepository;
}
