/********************************************************************************
 *                                                                              *
 * FitsIP - kernel filter                                                       *
 *                                                                              *
 * modified: 2025-02-08                                                         *
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

#ifndef OPKERNEL_H
#define OPKERNEL_H

#include <fitsip/core/opplugin.h>
#include <fitsip/core/kernel.h>
#include <QObject>

class OpKernelDialog;

class OpKernel: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  OpKernel();
  virtual ~OpKernel() override;

  virtual QString getMenuEntry() const override;

#ifdef USE_PYTHON
  virtual void bindPython(void* m) const override;
#endif

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

  void convolve(std::shared_ptr<FitsImage> image, const Kernel& kernel) const;

private:
  OpKernelDialog* dlg;
};


#endif // OPKERNEL_H
