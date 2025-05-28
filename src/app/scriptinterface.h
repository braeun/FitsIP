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

#include <fitsip/core/filelist.h>
#include <fitsip/core/fitsobject.h>
#include <memory>

class ScriptInterface
{
public:
  ScriptInterface();

  virtual void setWorkingDir(const std::string& dir) = 0;

  virtual std::string getWorkingDir() const = 0;

  virtual std::shared_ptr<FitsObject> get(const std::string& filename) = 0;

  virtual const std::vector<std::shared_ptr<FitsObject>>& getOpen() const = 0;

  virtual std::shared_ptr<FitsObject> load(const std::string& filename) = 0;

  virtual bool save(std::shared_ptr<FitsObject> obj, const std::string& filename) = 0;

  virtual void display(std::shared_ptr<FitsObject> obj) = 0;

  virtual std::shared_ptr<FileList> getSelectedFileList() const = 0;

};

#endif // SCRIPTINTERFACE_H
