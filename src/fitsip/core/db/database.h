#ifndef DATABASE_H
#define DATABASE_H

#include "camera.h"
#include "telescope.h"
#include <memory>
#include <map>
#include <vector>

class Database
{
public:
  Database();

  virtual std::vector<QString> getCameraList() = 0;

  virtual  std::vector<Camera> getCameras() = 0;

  virtual Camera getCamera(QString name) = 0;

  virtual bool removeCamera(QString name) = 0;

  virtual bool addCamera(const Camera& c) = 0;

  virtual bool updateCamera(const Camera& c) = 0;

  virtual std::vector<QString> getTelescopeList() = 0;

  virtual  std::vector<Telescope> getTelescopes() = 0;

  virtual Telescope getTelescope(QString name) = 0;

  virtual bool removeTelescope(QString name) = 0;

  virtual bool addTelescope(const Telescope& c) = 0;

  virtual bool updateTelescope(const Telescope& c) = 0;

  static Database* get(QString name="");

  static QStringList getDrivers();

private:
  static std::map<QString,std::unique_ptr<Database>> databases;
};

#endif // DATABASE_H
