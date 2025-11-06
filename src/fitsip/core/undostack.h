/********************************************************************************
 *                                                                              *
 * FitsIP - stack for undo operations                                           *
 *                                                                              *
 * modified: 2025-11-06                                                         *
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

#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include "fitsimage.h"
#include <QObject>
#include <deque>

class UndoStack: public QObject
{
  Q_OBJECT
public:
  UndoStack();

  size_t getMaxDepth() const;

  void setMaxDepth(size_t d);

  bool isUndoAvailable() const;

  void push(const FitsImage& img);

  FitsImage pop();

signals:
  void undoAvailable(bool flag);

private:
  size_t maxdepth;
  std::deque<FitsImage> stack;
};

#endif // UNDOSTACK_H
