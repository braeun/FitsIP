/********************************************************************************
 *                                                                              *
 * FitsIP - python scripting                                                    *
 *                                                                              *
 * modified: 2025-02-19                                                         *
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

#include "script.h"
#include <memory>

class PluginFactory;
class ScriptInterface;

#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
namespace py = pybind11;

class PythonScript: public Script
{
  Q_OBJECT
public:
  PythonScript(ScriptInterface* intf, PluginFactory* plugins);
  virtual ~PythonScript() override;

  virtual void runCmd(const QString& cmd) override;

  virtual void runFile(const QString& filename) override;

private:
  void init(ScriptInterface* intf, PluginFactory* plugins);
  void streamStdout(const char* s);
  void streamStderr(const char* s);
  void redirect(py::object, const char* pipe);
  void bind(py::module_& m, ScriptInterface* intf);
  void bindFitsObject(py::module_& m);
  void bindLists(py::module_& m, ScriptInterface* intf);
  void bindKernel(py::module_& m);
  void bindStatistics(py::module_& m);

  std::unique_ptr<py::scoped_interpreter> terp;

};

#endif // PYTHONSCRIPT_H
