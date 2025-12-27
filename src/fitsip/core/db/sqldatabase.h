#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include "database.h"
#include <QSqlError>

class SQLDatabase: public Database
{
public:
  SQLDatabase(QString name, QString driver, QString host, QString user, QString pwd);

  virtual QStringList getCameraList() override;

  virtual  std::vector<Camera> getCameras() override;

  virtual Camera getCamera(QString name) override;

  virtual bool removeCamera(QString name) override;

  virtual bool addCamera(const Camera& c) override;

  virtual bool updateCamera(const Camera& c) override;

  virtual QStringList getTelescopeList() override;

  virtual  std::vector<Telescope> getTelescopes() override;

  virtual Telescope getTelescope(QString name) override;

  virtual bool removeTelescope(QString name) override;

  virtual bool addTelescope(const Telescope& c) override;

  virtual bool updateTelescope(const Telescope& c) override;

  static Database* get(QString name="");

  static QStringList getDrivers();

private:
  void assertTables();

  QString connectionName;
  QSqlError lastError;

};

#endif // SQLDATABASE_H
