#ifndef EXTERNALTOOLSLAUNCHER_H
#define EXTERNALTOOLSLAUNCHER_H

#include <memory>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

class ExternalToolsLauncher
{
public:
  ExternalToolsLauncher();

  static QMimeType getMimeType(const QFileInfo& file);

  bool open(const QFileInfo& file);

private:

  QString findApplication(const QFileInfo& file);
  QString setFilename(QString program, const QString& filename);

  static QMimeDatabase db;
  static QString officeDocuments[];
};

#endif // EXTERNALTOOLSLAUNCHER_H
