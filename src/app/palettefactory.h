/********************************************************************************
 *                                                                              *
 * FitsIP - factory for the different color palettes of the GUI                 *
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

#ifndef PALETTEFACTORY_H
#define PALETTEFACTORY_H

#include <QPalette>
#include <QString>
#include <vector>

class PaletteFactory
{
public:

  static std::vector<QString> getPaletteNames();

  static QPalette getPalette(const QString& name);

  static void registerDefaultPalette(const QPalette& p);

private:
  PaletteFactory();

  static QPalette darkPalette();

  static QPalette defaultPalette;
};

#endif // PALETTEFACTORY_H
