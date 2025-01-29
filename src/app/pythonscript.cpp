#include "pythonscript.h"
#include "scriptinterface.h"
#include <fitsbase/pythonbase.h>
#include <iostream>
#include <functional>

PYBIND11_EMBEDDED_MODULE(fits, m) {
  pythonbase::bind(m);
  ScriptInterface::bind(m);
}



std::unique_ptr<PythonScript> PythonScript::instance;

PythonScript::PythonScript()
{
  init();
}

void PythonScript::runCmd(const QString& cmd)
{
//  std::cout << "python: " << cmd.toStdString() << std::endl;
  py::exec(cmd.toStdString());
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
  py::exec("import fits");
}

void PythonScript::streamStdout(const char* s)
{
  emit stdoutAvailable(s);
  std::cout << ">" << s;
}

void PythonScript::streamStderr(const char* s)
{
  emit stderrAvailable(s);
  std::cerr << "!" << s;
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
