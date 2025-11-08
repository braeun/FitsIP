#include "telescope.h"

Telescope::Telescope():
  name(""),
  f(0),
  d(0)
{
}

Telescope::Telescope(const QString& name, double f, double d) : name(name),
  f(f),
  d(d)
{
}

const QString& Telescope::getName() const
{
  return name;
}

void Telescope::setName(const QString& newName)
{
  name = newName;
}

double Telescope::getF() const
{
  return f;
}

void Telescope::setF(double newF)
{
  f = newF;
}

double Telescope::getD() const
{
  return d;
}

void Telescope::setD(double newD)
{
  d = newD;
}
