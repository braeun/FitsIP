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

#include "simpleprofiler.h"
#include <sys/time.h>
#include <sstream>

SimpleProfiler::SimpleProfiler():
  name("Default"),
  tstart({0,0}),
  tstop({0,0})
{
}

SimpleProfiler::SimpleProfiler(const std::string& n):
  name(n),
  tstart({0,0}),
  tstop({0,0})
{
}

const std::string& SimpleProfiler::getName() const
{
  return name;
}

bool SimpleProfiler::isRunning() const
{
  return (tstart.tv_sec) > 0 && (tstop.tv_sec == 0);
}

void SimpleProfiler::start()
{
  gettimeofday(&tstart,nullptr);
  tstop = {0,0};
}

void SimpleProfiler::stop()
{
  if (isRunning())
  {
    gettimeofday(&tstop,nullptr);
  }
}

int64_t SimpleProfiler::getDuration() const
{
  if (isRunning())
  {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t mStart = static_cast<int64_t>(tstart.tv_sec) * 1000000L + static_cast<int64_t>(tstart.tv_usec);
    int64_t mStop = static_cast<int64_t>(tv.tv_sec) * 1000000L + static_cast<int64_t>(tv.tv_usec);
    return mStop - mStart;
  }
  int64_t mStart = static_cast<int64_t>(tstart.tv_sec) * 1000000L + static_cast<int64_t>(tstart.tv_usec);
  int64_t mStop = static_cast<int64_t>(tstop.tv_sec) * 1000000L + static_cast<int64_t>(tstop.tv_usec);
  return mStop - mStart;
}

std::string SimpleProfiler::toString() const
{
  char buffer[64];
  double d = getDuration();
  if (d > 1000000)
  {
    d /= 1000000.0;
    snprintf(buffer,sizeof(buffer),"%.1fs",d);
  }
  else if (d > 1000)
  {
    d /= 1000.0;
    snprintf(buffer,sizeof(buffer),"%.1fms",d);
  }
  else
  {
    snprintf(buffer,sizeof(buffer),"%ldµs",static_cast<int64_t>(d));
  }
  std::ostringstream s;
  s << "[" << name << "] total execution time: " << buffer;
  return s.str();
}

