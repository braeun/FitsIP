#ifndef CAMERA_H
#define CAMERA_H

#include <QString>

class Camera
{
public:
  Camera();
  Camera(const QString& name, double pixelwidth, double pixelheight);

  const QString& getName() const;

  void setName(const QString& newName);

  double getPixelwidth() const;

  void setPixelwidth(double newPixelwidth);

  double getPixelheight() const;

  void setPixelheight(double newPixelheight);

  operator bool() const { return !name.isEmpty(); }

private:
  QString name;
  double pixelwidth;
  double pixelheight;
};

#endif // CAMERA_H
