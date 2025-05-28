/********************************************************************************
 *                                                                              *
 * FitsIP - database configuration                                              *
 *                                                                              *
 * modified: 2025-03-13                                                         *
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

#ifndef DB_H
#define DB_H

#include <vector>
#include <QString>

class Settings;

namespace db
{

struct Camera
{
  QString name;
  double pixelwidth;
  double pixelheight;
};

extern const char* CONNECTION_NAME;

extern void configure(const Settings& settings);

extern std::vector<Camera> getCameras();

extern void addCamera(const Camera& c);

extern void updateCamera(const Camera& c);


} // namespace

#endif // DB_H
