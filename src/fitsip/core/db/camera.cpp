#include "camera.h"

Camera::Camera():
  name(""),
  pixelwidth(0),
  pixelheight(0)
{
}

Camera::Camera(const QString& name, double pixelwidth, double pixelheight):
  name(name),
  pixelwidth(pixelwidth),
  pixelheight(pixelheight)
{
}

const QString& Camera::getName() const
{
  return name;
}

void Camera::setName(const QString& newName)
{
  name = newName;
}

double Camera::getPixelwidth() const
{
  return pixelwidth;
}

void Camera::setPixelwidth(double newPixelwidth)
{
  pixelwidth = newPixelwidth;
}

double Camera::getPixelheight() const
{
  return pixelheight;
}

void Camera::setPixelheight(double newPixelheight)
{
  pixelheight = newPixelheight;
}
