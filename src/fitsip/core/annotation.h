#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QString>

class Annotation
{
public:
  Annotation();
  Annotation(int xp, int yp, const QString& txt);
  Annotation(int xp, int yp, double x, double y, const QString& txt);

  int getXp() const;

  void setXp(int newXp);

  int getYp() const;

  void setYp(int newYp);

  double getX() const;

  void setX(double newX);

  double getY() const;

  void setY(double newY);

  QString getText() const;

  void setText(const QString& newTxt);

private:
  int xp;
  int yp;
  double x;
  double y;
  QString txt;
};


#endif // ANNOTATION_H
