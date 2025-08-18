#include "hanningwindow.h"
#include <cmath>

HanningWindow::HanningWindow(int m)
{
  for (int n=0;n<=m;++n)
  {
    w.push_back(0.5*(1-cos(2*M_PI*n/m)));
  }
}

const std::vector<double>& HanningWindow::get() const
{
  return w;
}
