/********************************************************************************
 *                                                                              *
 * FitsIP - interface function for scripts                                      *
 *                                                                              *
 * modified: 2025-02-05                                                         *
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

#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#include <fitsbase/fitsobject.h>
#include <memory>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif


class ScriptInterface: public QObject
{
  Q_OBJECT
public:

  void setWorkingDir(const QString& dir);

  /**
   * @brief Change the working directory and emit the correspondign signal.
   *
   * This method ist used by scripts to change the working directory.
   * @param dir the new owrking directory.
   */
  void changeWorkingDir(const std::string& dir);

  std::shared_ptr<FitsObject> get(const std::string& filename);

  std::shared_ptr<FitsObject> load(const std::string& filename);

  bool save(std::shared_ptr<FitsObject> obj, const std::string& filename);

  void display(std::shared_ptr<FitsObject> obj);

#ifdef USE_PYTHON
  static void bind(py::module_& m);
#endif

  static ScriptInterface* getInterface();

signals:
  void display(int id);

  void workingDirChanged(const QString& dir);

private:
  ScriptInterface();

  QString workingdir;

  static std::unique_ptr<ScriptInterface> interface;
};

#endif // SCRIPTINTERFACE_H
