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

#include "pythonscript.h"
#include "scriptinterface.h"
#include <fitsbase/pluginfactory.h>
#include <fitsbase/pythonbase.h>
#include <iostream>
#include <functional>
#include <QFileInfo>

PYBIND11_EMBEDDED_MODULE(fits, m) {
  m.attr("scriptpath") = "";
  pythonbase::bind(m);
  ScriptInterface::bind(m);
  PluginFactory::instance()->setBinding(&m);
}



std::unique_ptr<PythonScript> PythonScript::instance;

PythonScript::PythonScript()
{
  init();
}

void PythonScript::runCmd(const QString& cmd)
{
//  std::cout << "python: " << cmd.toStdString() << std::endl;
  auto fits_module = py::module_::import("fits");
  fits_module.attr("scriptpath") = "";
  py::exec(cmd.toStdString());
}

void PythonScript::runFile(const QString& filename)
{
  QFileInfo info(filename);
  auto fits_module = py::module_::import("fits");
  fits_module.attr("scriptpath") = info.absolutePath().toStdString();
  py::eval_file(filename.toStdString());
}

PythonScript* PythonScript::getInstance()
{
  if (!instance)
  {
    instance.reset(new PythonScript());
  }
  return instance.get();
}

void PythonScript::init()
{
  if (terp) terp.reset();
  terp.reset(new py::scoped_interpreter);
  redirect(pybind11::cpp_function([this](const char* s){streamStdout(s);}),"stdout");
  redirect(pybind11::cpp_function([this](const char* s){streamStderr(s);}),"stderr");
//  auto fits_module = py::module_::import("fits");
  py::exec("import fits");
}

void PythonScript::streamStdout(const char* s)
{
  emit stdoutAvailable(s);
}

void PythonScript::streamStderr(const char* s)
{
  emit stderrAvailable(s);
}


/*
 * based on
 * https://github.com/TinyTinni/PyLogHook
 * Copyright(c) 2017 Matthias Möller
 * MIT License
 */

void PythonScript::redirect(py::object callback, const char* pipe)
{
  PyObject *out = PySys_GetObject(pipe);

  // in case python could not create stdout/stderr
  // this happens in some gui application
  // just register a new nameless type for this
  if (out == nullptr || out == Py_None)
  {
      std::string register_read_write = std::string("import sys\n\
sys.") + pipe + std::string(" = type(\"\",(object,),{\"write\":lambda self, txt: None, \"flush\":lambda self: None})()\n");
      PyRun_SimpleString(register_read_write.c_str());
      out = PySys_GetObject(pipe);
  }
  // overwrite write function
  PyObject_SetAttrString(out, "write",callback.ptr());
}
