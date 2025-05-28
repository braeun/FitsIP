#ifndef LOGBOOKUTILE_H
#define LOGBOOKUTILE_H

#include "logbookstorage.h"
#include <QString>
#include <memory>

namespace logbookutils
{

extern QString getFilenameFilter();

extern std::unique_ptr<LogbookStorage> getLogboook(QString filename);

}

#endif // LOGBOOKUTILE_H
