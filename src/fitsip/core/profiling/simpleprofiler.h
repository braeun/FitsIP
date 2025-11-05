/********************************************************************************
 *                                                                              *
 * FitsIP - a simple profiler                                                   *
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

#ifndef SIMPLEPROFILER_H
#define SIMPLEPROFILER_H

#include <cstdint>
#include <string>

class SimpleProfiler
{
public:
  SimpleProfiler();
  SimpleProfiler(const std::string& name);

  const std::string& getName() const;

  bool isRunning() const;

  void start();

  void stop();

  /**
   * @brief Get the duration in microseconds.
   *
   * This will get the time difference between the stop and the start of the
   * profile in microseconds. If the profiler is still running, the time
   * difference between the current time and the start will be returned.
   * @return the duration in microseconds
   */
  int64_t getDuration() const;

  /**
   * @brief Get the duration in microseconds as a formatted string for direct printing.
   *
   * This will return a string containing the profiler's name as well as
   * the duraction. The duration will be the time difference between the
   * stop and the start of the profile in microseconds. If the profiler is
   * still running, the time difference between the current time and the
   * start will be used.
   * @return the formatted duration as a string
   */
  std::string toString() const;

private:
  std::string name;
  struct timeval tstart;
  struct timeval tstop;

};

#endif // SIMPLEPROFILER_H
