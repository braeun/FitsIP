#include "externaltoolslauncher.h"
#include "settings.h"
#include "io/iofactory.h"
#include <QDebug>
#include <QProcess>

QMimeDatabase ExternalToolsLauncher::db;

QString ExternalToolsLauncher::officeDocuments[] = {
  "application/vnd.oasis.opendocument.text",
  "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
  "application/msword"
};

ExternalToolsLauncher::ExternalToolsLauncher()
{
}


QMimeType ExternalToolsLauncher::getMimeType(const QFileInfo& file)
{
//  std::cout << qPrintable(db.mimeTypeForFile(filename).name()) << std::endl;
  return db.mimeTypeForFile(file.fileName());
}

bool ExternalToolsLauncher::open(const QFileInfo& file)
{
  QString program = findApplication(file);
  if (program.isEmpty())
  {
//    qWarning() << "No program found to edit file " << file.fileName();
    return false;
  }
  program = setFilename(program,file.absoluteFilePath());
  QStringList args = QProcess::splitCommand(program);
  program = args[0];
  args.removeFirst();
//  qInfo() << "Launch editor: " << program;
  QProcess* process = new QProcess;
  process->start(program,args);
  return true;
}


QString ExternalToolsLauncher::findApplication(const QFileInfo& file)
{
  Settings settings;
  if (file.isDir())
  {
    return settings.getTool(Settings::FileManager);
  }
  QMimeType mime = db.mimeTypeForFile(file.fileName());
  if (!mime.isValid()) return "";
  for (const QString& m : officeDocuments)
  {
    if (mime.inherits(m)) return settings.getTool(Settings::OfficeEditor);
  }
  if (IOFactory::getInstance()->isScript(file.fileName()))
  {
    return settings.getTool(Settings::ScriptEditor);
  }
  if (mime.inherits("text/plain"))
  {
    return settings.getTool(Settings::TextEditor);
  }
  return "";
}

QString ExternalToolsLauncher::setFilename(QString program, const QString& filename)
{
  if (program.contains("%F"))
  {
    program.replace("%F",filename);
  }
  else
  {
    program += " \"" + filename + "\"";
  }
  return program;
}

