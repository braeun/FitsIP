#ifndef PYTHONSCRIPT_H
#define PYTHONSCRIPT_H

#include <memory>
#include <QObject>
#include <QString>

#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace py = pybind11;

class PythonScript: public QObject
{
  Q_OBJECT
public:
  PythonScript();

  void runCmd(const QString& cmd);

  static PythonScript* getInstance();

signals:
  void stdoutAvailable(QString s);
  void stderrAvailable(QString s);

private:
  void init();
  void streamStdout(const char* s);
  void streamStderr(const char* s);
  void redirect(py::object, const char* pipe);

  std::unique_ptr<py::scoped_interpreter> terp;

  static std::unique_ptr<PythonScript> instance;
};

#endif // PYTHONSCRIPT_H
