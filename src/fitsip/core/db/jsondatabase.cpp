#include "jsondatabase.h"
#include "../settings.h"
#include <nlohmann/json.hpp>
#include <QDebug>
#include <QDir>
#include <algorithm>
#include <fstream>

JsonDatabase::JsonDatabase(QString name, QString driver, QString host, QString user, QString pwd)
{
  QDir dir(host);
  if (dir.isRelative())
  {
    host =Settings().getInternalDirectory() + "/" + host;
    dir = QDir(host);
  }
  if (!dir.exists())
  {
    dir.mkpath(dir.absolutePath());
  }
  QFileInfo info(name);
  if (info.suffix().isEmpty()) name += ".json";
  filename = dir.absoluteFilePath(name).toStdString();
  load();
}

std::vector<QString> JsonDatabase::getCameraList()
{
  std::vector<QString> list;
  for (const auto& c : cameras)
  {
    list.push_back(c.getName());
  }
  return list;
}

std::vector<Camera> JsonDatabase::getCameras()
{
  return cameras;
}

Camera JsonDatabase::getCamera(QString name)
{
  for (const auto& c : cameras)
  {
    if (c.getName() == name) return c;
  }
  return Camera();
}

bool JsonDatabase::removeCamera(QString name)
{
  for (auto it = cameras.begin(); it != cameras.end();)
  {
    if (it->getName() == name)
    {
      cameras.erase(it);
      break;
    }
  }
  return save();
}

bool JsonDatabase::addCamera(const Camera& camera)
{
  Camera c = getCamera(camera.getName());
  if (c)
  {
    qWarning() << "Camera with name" << camera.getName() << "already exists";
    return false;
  }
  cameras.push_back(camera);
  std::sort(cameras.begin(),cameras.end(),[](const Camera& c1, const Camera& c2){return c1.getName()<c2.getName();});
  qInfo() << "Added camera: " << camera.getName();
  return save();
}

bool JsonDatabase::updateCamera(const Camera& c)
{
  for (auto& camera : cameras)
  {
    if (camera.getName() == c.getName())
    {
      camera.setPixelwidth((c.getPixelwidth()));
      camera.setPixelheight(c.getPixelheight());
      qInfo() << "Updated camera: " << camera.getName();
      break;
    }
  }
  return save();
}


QStringList JsonDatabase::getDrivers()
{
  return QStringList("JSON");
}







bool JsonDatabase::load()
{
  std::ifstream s(filename);
  if (s.is_open() && !s.fail())
  {
    cameras.clear();
    nlohmann::json json = nlohmann::json::parse(s);
    if (json.contains("cameras"))
    {
      for (const auto& tmp : json.at("cameras"))
      {
        std::string name = tmp.value("name",std::string());
        double pw = tmp.value("pixelwidth",1.0);
        double ph = tmp.value("pixelheight",1.0);
        cameras.push_back(Camera(QString::fromStdString(name),pw,ph));
      }
    }
    s.close();
    return true;
  }
  s.close();
  return false;
}

bool JsonDatabase::save()
{
  nlohmann::json json;
  nlohmann::json jc;
  for (const auto& camera : cameras)
  {
    nlohmann::json c;
    c["name"] = camera.getName().toStdString();
    c["pixelwidth"] = camera.getPixelwidth();
    c["pixelheight"] = camera.getPixelheight();
    jc.push_back(c);
  }
  json["cameras"] = jc;
  std::ofstream s(filename);
  if (!s.fail())
  {
    s << std::setw(2) << json;
    s.close();
    return true;
  }
  return false;
}



