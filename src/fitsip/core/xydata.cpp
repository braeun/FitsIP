#include "xydata.h"

XYData::XYData():
  type(Curve)
{
}

XYData::XYData(const QString& title, const std::vector<double>& x, const std::vector<double>& y):
  type(Curve),
  title(title),
  x(x),
  y(y)
{
}

XYData::Type XYData::getType() const
{
  return type;
}

void XYData::setType(Type newType)
{
  type = newType;
}

const QString& XYData::getTitle() const
{
  return title;
}

void XYData::setTitle(const QString& newTitle)
{
  title = newTitle;
}

const QString& XYData::getXAxis() const
{
  return xAxis;
}

void XYData::setXAxis(const QString& newXAxis)
{
  xAxis = newXAxis;
}

const QString& XYData::getYAxis() const
{
  return yAxis;
}

void XYData::setYAxis(const QString& newYAxis)
{
  yAxis = newYAxis;
}

const std::vector<double>& XYData::getX() const
{
  return x;
}

void XYData::setX(const std::vector<double>& newX)
{
  x = newX;
}

const std::vector<double>& XYData::getY() const
{
  return y;
}

void XYData::setY(const std::vector<double>& newY)
{
  y = newY;
}

