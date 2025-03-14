/********************************************************************************
 *                                                                              *
 * FitsIP - database configuration                                              *
 *                                                                              *
 * modified: 2025-03-13                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of FitsIP.                                                 *
 * FitsIP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * FitsIP is distributed in the hope that it will be useful, but                *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * FitsIP. If not, see <https://www.gnu.org/licenses/>.                         *
 ********************************************************************************/

#include "db.h"
#include "../settings.h"
#include <stdexcept>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace db
{

const char* CONNECTION_NAME = "FITSDB";

//const char* CAMERA_TABLE = "cameras";
//const char* CAMERA_NAME = "name";
//const char* CAMERA_PIXEL_WIDTH = "pixelwidth";
//const char* CAMERA_PIXEL_HEIGHT = "pixelheight";


void assertCameraTable()
{
  QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
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
  db.close();
}

void configure(const Settings& settings)
{
  if (QSqlDatabase::contains(CONNECTION_NAME))
  {
    QSqlDatabase::database(CONNECTION_NAME,false).close();
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
  }
  QString driver = settings.getDatabaseDriver();
  QString name = settings.getDatabase();
  QString host = settings.getDatabaseHost();
  QString user = settings.getDatabaseUser();
  QString password = settings.getDatabasePassword();
  QSqlDatabase db = QSqlDatabase::addDatabase(driver,CONNECTION_NAME);
  if (driver == "QSQLITE")
  {
    QFileInfo info(name);
    if (!info.isAbsolute())
    {
      QDir dir(settings.getInternalDirectory());
      name = dir.absoluteFilePath(name);
      info = QFileInfo(name);
      dir.mkpath(info.absolutePath());
    }
  }
  db.setDatabaseName(name);
  db.setHostName(host);
  db.setUserName(user);
  db.setPassword(password);
  assertCameraTable();
}

std::vector<Camera> getCameras()
{
  std::vector<Camera> list;
  QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
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
      Camera c;
      c.name = q.value(0).toString();
      c.pixelwidth = q.value(1).toDouble();
      c.pixelheight = q.value(2).toDouble();
      list.push_back(c);
    }
  }
  db.close();
  return list;
}

void addCamera(const Camera& c)
{
  QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return;
  }
  QSqlQuery q("insert into cameras (name,pixelwidth,pixelheight) values (?,?,?)",db);
  q.bindValue(0,c.name);
  q.bindValue(1,c.pixelwidth);
  q.bindValue(2,c.pixelheight);
  if (!q.exec())
  {
    qWarning() << q.lastError();
  }
  else
  {
    qInfo() << "Added camera: " << c.name;
  }
}

void updateCamera(const Camera& c)
{
  QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
  if (!(db.isValid() && db.isOpen()))
  {
    qWarning() << "Database not configured";
    return;
  }
  QSqlQuery q("update cameras set pixelwidth=?,pixelheight=? where name=?",db);
  q.bindValue(0,c.pixelwidth);
  q.bindValue(1,c.pixelheight);
  q.bindValue(2,c.name);
  if (!q.exec())
  {
    qWarning() << q.lastError();
  }
  else
  {
    qInfo() << "Updated camera: " << c.name;
  }
}



} // namespace
