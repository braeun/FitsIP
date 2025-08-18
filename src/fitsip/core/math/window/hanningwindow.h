#ifndef HANNINGWINDOW_H
#define HANNINGWINDOW_H

#include <vector>

class HanningWindow
{
public:
  HanningWindow(int m);

  const std::vector<double>& get() const;

  double operator[](std::size_t i) const { return w[i]; }

private:
  std::vector<double> w;
};

#endif // HANNINGWINDOW_H
