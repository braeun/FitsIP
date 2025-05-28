
#include "logbookutils.h"
#include "xmllogbookstorage.h"
#include <QFileInfo>

namespace logbookutils
{

QString getFilenameFilter()
{
  QString f = "Logbook (*.lbk *.log *.xml)";
  f += ";;All files (*)";
  return f;
}

std::unique_ptr<LogbookStorage> getLogboook(QString filename)
{
  QFileInfo info(filename);
  QString e = info.suffix();
  if (e == "xml" || e == "lbk" || e == "log") return std::make_unique<XMLLogbookStorage>(filename);
  return std::unique_ptr<LogbookStorage>();
}

}

