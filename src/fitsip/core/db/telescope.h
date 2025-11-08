#ifndef TELESCOPE_H
#define TELESCOPE_H

#include <QString>

class Telescope
{
public:
  Telescope();
  Telescope(const QString& name, double f, double d);

  const QString& getName() const;

  void setName(const QString& newName);

  double getF() const;

  void setF(double newF);

  double getD() const;

  void setD(double newD);

  operator bool() const { return !name.isEmpty(); }

private:
  QString name;
  double f;
  double d;
};

#endif // TELESCOPE_H
