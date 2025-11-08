#ifndef JSONDATABASE_H
#define JSONDATABASE_H

#include "database.h"
#include <string>

class JsonDatabase: public Database
{
public:
  JsonDatabase(QString name, QString driver, QString host, QString user, QString pwd);

  virtual std::vector<QString> getCameraList() override;

  virtual  std::vector<Camera> getCameras() override;

  virtual Camera getCamera(QString name) override;

  virtual bool removeCamera(QString name) override;

  virtual bool addCamera(const Camera& c) override;

  virtual bool updateCamera(const Camera& c) override;

  virtual std::vector<QString> getTelescopeList() override;

  virtual  std::vector<Telescope> getTelescopes() override;

  virtual Telescope getTelescope(QString name) override;

  virtual bool removeTelescope(QString name) override;

  virtual bool addTelescope(const Telescope& c) override;

  virtual bool updateTelescope(const Telescope& c) override;

  static QStringList getDrivers();

private:
  bool load();
  bool save();

  std::string filename;
  std::vector<Camera> cameras;
  std::vector<Telescope> telescopes;
};

#endif // JSONDATABASE_H
