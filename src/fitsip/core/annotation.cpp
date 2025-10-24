#include "annotation.h"

Annotation::Annotation():
  xp(-1),
  yp(-1),
  x(-1),
  y(-1)
{
}

Annotation::Annotation(int xp, int yp, const QString& txt):
  xp(xp),
  yp(yp),
  x(-1),
  y(-1),
  txt(txt)
{
}

Annotation::Annotation(int xp, int yp, double x, double y, const QString& txt):
  xp(xp),
  yp(yp),
  x(x),
  y(y),
  txt(txt)
{
}

int Annotation::getXp() const
{
  return xp;
}

void Annotation::setXp(int newXp)
{
  xp = newXp;
}

int Annotation::getYp() const
{
  return yp;
}

void Annotation::setYp(int newYp)
{
  yp = newYp;
}

double Annotation::getX() const
{
  return x;
}

void Annotation::setX(double newX)
{
  x = newX;
}

double Annotation::getY() const
{
  return y;
}

void Annotation::setY(double newY)
{
  y = newY;
}

QString Annotation::getText() const
{
  return txt;
}

void Annotation::setText(const QString& newTxt)
{
  txt = newTxt;
}

