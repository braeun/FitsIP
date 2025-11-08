#ifndef JSONDATABASE_H
#define JSONDATABASE_H

#include "database.h"
#include <string>

class JsonDatabase: public Database
{
public:
  JsonDatabase(QString name, QString driver, QString host, QString user, QString pwd);

  virtual std::vector<QString> getCameraList();

  virtual  std::vector<Camera> getCameras();

  virtual Camera getCamera(QString name);

  virtual bool removeCamera(QString name);

  virtual bool addCamera(const Camera& c);

  virtual bool updateCamera(const Camera& c);

  static QStringList getDrivers();

private:
  bool load();
  bool save();

  std::string filename;
  std::vector<Camera> cameras;
};

#endif // JSONDATABASE_H
