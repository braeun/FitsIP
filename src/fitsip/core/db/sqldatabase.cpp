#include "sqldatabase.h"
#include "../settings.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>

SQLDatabase::SQLDatabase(QString name, QString driver, QString host, QString user, QString pwd)
{
  /* create a unique connection name base on the current time in ms */
  connectionName = QString::number(QDateTime::currentMSecsSinceEpoch());
  while (QSqlDatabase::contains(connectionName))
  {
    connectionName = "_" + connectionName;
  }
  qDebug() << "DeviceSql: " << connectionName;
  QSqlDatabase db = QSqlDatabase::addDatabase(driver,connectionName);
  if (driver == "QSQLITE")
  {
    QFileInfo info(name);
    if (!info.isAbsolute())
    {
      QDir dir(Settings().getInternalDirectory());
      name = dir.absoluteFilePath(name);
      info = QFileInfo(name);
      dir.mkpath(info.absolutePath());
    }
  }
  db.setDatabaseName(name);
  db.setHostName(host);
  db.setUserName(user);
  db.setPassword(pwd);
  bool ret = db.open();
  if (!ret)
  {
    qWarning() << "open db result: " << db.isValid() << " " << db.isOpen();
    qWarning() << "              : " << db.lastError();
  }
  assertTables();
}

std::vector<QString> SQLDatabase::getCameraList()
{
  std::vector<QString> list;
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return list;
  }
  QSqlQuery q("select name from cameras order by name",db);
  if (q.exec())
  {
    while (q.next())
    {
      list.push_back(q.value(0).toString());
    }
  }
  return list;
}

std::vector<Camera> SQLDatabase::getCameras()
{
  std::vector<Camera> list;
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return list;
  }
  QSqlQuery q("select name,pixelwidth,pixelheight from cameras order by name",db);
  if (q.exec())
  {
    while (q.next())
    {
      Camera c(q.value(0).toString(),q.value(1).toDouble(),q.value(2).toDouble());
      list.push_back(c);
    }
  }
  return list;
}

Camera SQLDatabase::getCamera(QString name)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return Camera();
  }
  QSqlQuery q("select pixelwidth,pixelheight from cameras where name=?",db);
  q.bindValue(0,name);
  if (q.exec())
  {
    if (q.next())
    {
      double w = q.value(0).toDouble();
      double h = q.value(1).toDouble();
      Camera c(name,w,h);
      return c;
    }
  }
  return Camera();
}

bool SQLDatabase::removeCamera(QString name)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return false;
  }
  QSqlQuery q("delete from cameras where name=?",db);
  q.bindValue(0,name);
  if (!q.exec())
  {
    qWarning() << q.lastError();
    return false;
  }
  return true;
}

bool SQLDatabase::addCamera(const Camera& c)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return false;
  }
  QSqlQuery q("insert into cameras (name,pixelwidth,pixelheight) values (?,?,?)",db);
  q.bindValue(0,c.getName());
  q.bindValue(1,c.getPixelwidth());
  q.bindValue(2,c.getPixelheight());
  if (!q.exec())
  {
    qWarning() << q.lastError();
    return false;
  }
  qInfo() << "Added camera: " << c.getName();
  return true;
}

bool SQLDatabase::updateCamera(const Camera& c)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return false;
  }
  QSqlQuery q("update cameras set pixelwidth=?,pixelheight=? where name=?",db);
  q.bindValue(0,c.getPixelwidth());
  q.bindValue(1,c.getPixelheight());
  q.bindValue(2,c.getName());
  if (!q.exec())
  {
    qWarning() << q.lastError();
    return false;
  }
  qInfo() << "Updated camera: " << c.getName();
  return true;
}

std::vector<QString> SQLDatabase::getTelescopeList()
{
  std::vector<QString> list;
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return list;
  }
  QSqlQuery q("select name from telescopes order by name",db);
  if (q.exec())
  {
    while (q.next())
    {
      list.push_back(q.value(0).toString());
    }
  }
  return list;
}

std::vector<Telescope> SQLDatabase::getTelescopes()
{
  std::vector<Telescope> list;
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return list;
  }
  QSqlQuery q("select name,f,d from telescopes order by name",db);
  if (q.exec())
  {
    while (q.next())
    {
      Telescope c(q.value(0).toString(),q.value(1).toDouble(),q.value(2).toDouble());
      list.push_back(c);
    }
  }
  return list;
}

Telescope SQLDatabase::getTelescope(QString name)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return Telescope();
  }
  QSqlQuery q("select f,d from telescopes where name=?",db);
  q.bindValue(0,name);
  if (q.exec())
  {
    if (q.next())
    {
      double f = q.value(0).toDouble();
      double d = q.value(1).toDouble();
      return Telescope(name,f,d);
    }
  }
  return Telescope();
}

bool SQLDatabase::removeTelescope(QString name)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return false;
  }
  QSqlQuery q("delete from telescopes where name=?",db);
  q.bindValue(0,name);
  if (!q.exec())
  {
    qWarning() << q.lastError();
    return false;
  }
  return true;
}

bool SQLDatabase::addTelescope(const Telescope& t)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return false;
  }
  QSqlQuery q("insert into telescopes (name,f,d) values (?,?,?)",db);
  q.bindValue(0,t.getName());
  q.bindValue(1,t.getF());
  q.bindValue(2,t.getD());
  if (!q.exec())
  {
    qWarning() << q.lastError();
    return false;
  }
  qInfo() << "Added telescope: " << t.getName();
  return true;
}

bool SQLDatabase::updateTelescope(const Telescope& c)
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return false;
  }
  QSqlQuery q("update telescopes set f=?,d=? where name=?",db);
  q.bindValue(0,c.getF());
  q.bindValue(1,c.getD());
  q.bindValue(2,c.getName());
  if (!q.exec())
  {
    qWarning() << q.lastError();
    return false;
  }
  qInfo() << "Updated telescope: " << c.getName();
  return true;
}



QStringList SQLDatabase::getDrivers()
{
  return QSqlDatabase::drivers();
}





void SQLDatabase::assertTables()
{
  QSqlDatabase db = QSqlDatabase::database(connectionName);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return;
  }
  if (!db.tables().contains("cameras"))
  {
    QSqlQuery q(db);
    QString s = QString("create table cameras (")
                + "name varchar(128) not null,"
                + "pixelwidth real,"
                + "pixelheight real"
                + ")";
    if (!q.exec(s)) qWarning() << q.lastError().text();
  }
  if (!db.tables().contains("telescopes"))
  {
    QSqlQuery q(db);
    QString s = QString("create table telescopes (")
                + "name varchar(128) not null,"
                + "f real,"
                + "d real"
                + ")";
    if (!q.exec(s)) qWarning() << q.lastError().text();
  }
  db.close();
}

