#include "database.h"
#include "jsondatabase.h"
#include "sqldatabase.h"
#include "../settings.h"

std::map<QString,std::unique_ptr<Database>> Database::databases;

Database::Database()
{
}

Database* Database::get(QString name)
{
  Settings settings;
  if (name.isEmpty())
  {
    name = settings.getDatabase();
  }
  if (databases.find(name) == databases.end())
  {
    std::unique_ptr<Database> db;
    QString driver = settings.getDatabaseDriver();
    if (JsonDatabase::getDrivers().contains(driver))
    {
      db = std::make_unique<JsonDatabase>(name,driver,settings.getDatabaseHost(),
                                          settings.getDatabaseUser(),settings.getDatabasePassword());
    }
    else
    {
      db = std::make_unique<SQLDatabase>(name,driver,settings.getDatabaseHost(),
                                            settings.getDatabaseUser(),settings.getDatabasePassword());
    }
    databases[name] = std::move(db);
  }
  return databases[name].get();
}

QStringList Database::getDrivers()
{
  QStringList list;
  list << JsonDatabase::getDrivers();
  list << SQLDatabase::getDrivers();
  return list;
}
