/********************************************************************************
 *                                                                              *
 * FitsIP - python scripting                                                    *
 *                                                                              *
 * modified: 2025-01-31                                                         *
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

  void runFile(const QString& filename);

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
