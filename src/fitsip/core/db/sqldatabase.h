#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include "database.h"
#include <QSqlError>

class SQLDatabase: public Database
{
public:
  SQLDatabase(QString name, QString driver, QString host, QString user, QString pwd);

  virtual std::vector<QString> getCameraList();

  virtual  std::vector<Camera> getCameras();

  virtual Camera getCamera(QString name);

  virtual bool removeCamera(QString name);

  virtual bool addCamera(const Camera& c);

  virtual bool updateCamera(const Camera& c);

  static QStringList getDrivers();

private:
  void assertTables();

  QString connectionName;
  QSqlError lastError;

};

#endif // SQLDATABASE_H
