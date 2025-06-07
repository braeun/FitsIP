
#include <fitsip/core/math/gaussfit.h>
#include <fitsip/core/math/mathfunctions.h>
#include <vector>
#include <iostream>

int main(int argc, char* argv[])
{
  int n = 200;
  ValueType amp = 100;
  ValueType sigma = 3.5;
  ValueType center = 75;
  std::vector<ValueType> x;
  std::vector<ValueType> y;
  for (int i=0;i<n;i++)
  {
    x.push_back(i);
    y.push_back(math_functions::gaussian(i,amp,center,sigma));
  }
  GaussFit fit;
  int ret = fit.fit(x,y);
  std::cout << "Gauss fit: ret = " << ret << std::endl;
  std::cout << "  center: " << fit.getCenter() << "  must be " << center << std::endl;
  std::cout << "  sigma : " << fit.getSigma() << "  must be " << sigma << std::endl;
  std::cout << "  ampl. : " << fit.getAmplitude() << "  must be " << amp << std::endl;
  return 0;
}
