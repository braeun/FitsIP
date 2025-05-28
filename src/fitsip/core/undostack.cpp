/********************************************************************************
 *                                                                              *
 * FitsIP - stack for undo operations                                           *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#include "undostack.h"

UndoStack::UndoStack():
  maxdepth(5)
{
}

size_t UndoStack::getMaxDepth() const
{
  return maxdepth;
}

void UndoStack::setMaxDepth(size_t d)
{
  maxdepth = d;
}

bool UndoStack::isUndoAvailable() const
{
  return !stack.empty();
}

void UndoStack::push(std::shared_ptr<FitsImage> img)
{
  if (stack.size() == maxdepth) stack.pop_front();
  stack.push_back(std::make_shared<FitsImage>(*img));
}

std::shared_ptr<FitsImage> UndoStack::pop()
{
  if (stack.empty()) return std::shared_ptr<FitsImage>();
  std::shared_ptr<FitsImage> p = stack.back();
  stack.pop_back();
  return p;
}
