#ifndef SCRIPT_H
#define SCRIPT_H

#include <QObject>
#include <QString>

class Script: public QObject
{
  Q_OBJECT
public:
  Script();

  virtual void runCmd(const QString& cmd) = 0;

  virtual void runFile(const QString& filename) = 0;

signals:
  void stdoutAvailable(QString s);
  void stderrAvailable(QString s);

};

#endif // SCRIPT_H
