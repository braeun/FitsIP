#ifndef XYDATA_H
#define XYDATA_H

#include <QString>
#include <vector>

class XYData
{
public:

  enum Type { Curve, Scatter };

  XYData();
  XYData(const QString& title, const std::vector<double>& x, const std::vector<double>& y);

  Type getType() const;

  void setType(Type newType);

  const QString& getTitle() const;

  void setTitle(const QString& newTitle);

  const QString& getXAxis() const;

  void setXAxis(const QString& newXAxis);

  const QString& getYAxis() const;

  void setYAxis(const QString& newYAxis);

  const std::vector<double>& getX() const;

  void setX(const std::vector<double>& newX);

  const std::vector<double>& getY() const;

  void setY(const std::vector<double>& newY);

private:
  Type type;
  QString title;
  QString xAxis;
  QString yAxis;
  std::vector<double> x;
  std::vector<double> y;
};

#endif // XYDATA_H
