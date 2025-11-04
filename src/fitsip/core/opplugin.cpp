/********************************************************************************
 *                                                                              *
 * FitsIP - base class for operation plugins                                    *
 *                                                                              *
 * modified: 2025-08-27                                                         *
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

#include "opplugin.h"
#include "settings.h"
#include <io/iofactory.h>
#include <QDir>

Q_LOGGING_CATEGORY(LOG_PROFILER,"profiler");

OpPlugin::OpPlugin():
  error("")
{
}

OpPlugin::~OpPlugin()
{
}

bool OpPlugin::requiresImage() const
{
  return true;
}

bool OpPlugin::requiresFileList() const
{
  return false;
}

std::vector<std::shared_ptr<FitsObject>> OpPlugin::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>();
}

OpPlugin::ResultType OpPlugin::execute(std::shared_ptr<FitsObject> /*image*/, const OpPluginData& /*data*/)
{
  log("OpPlugin::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data) - not implemented");
  return CANCELLED;
}

OpPlugin::ResultType OpPlugin::execute(const std::vector<QFileInfo>& /*list*/, const OpPluginData& /*data*/)
{
  log("OpPlugin::execute(const std::vector<QFileInfo>& list, const OpPluginData& data) - not implemented");
  return CANCELLED;
}

OpPlugin::ResultType OpPlugin::execute(const std::vector<std::shared_ptr<FitsObject>>& list, const OpPluginData& data)
{
  std::vector<QFileInfo> filelist;
  for (const std::shared_ptr<FitsObject>& file : list)
  {
    filelist.push_back(QFileInfo(file->getFilename()));
  }
  return execute(filelist,data);
}

const std::vector<QFileInfo> OpPlugin::getFileList() const
{
  return filelist;
}

QString OpPlugin::getError() const
{
  return error;
}

void OpPlugin::bindPython(void*) const
{
}

void OpPlugin::setError(const QString &err)
{
  error = err;
}

OpPlugin::ResultType OpPlugin::save(FitsImage* image, const QString& outputpath, const QFileInfo &info, const QString& tag)
{
  return save(std::make_shared<FitsObject>(std::make_shared<FitsImage>(*image)),outputpath,info,tag);
}

OpPlugin::ResultType OpPlugin::save(std::shared_ptr<FitsObject> image, const QString& outputpath, const QFileInfo &info, const QString& tag)
{
  QString fn = info.baseName();
  if (!tag.isEmpty()) fn+= "_" + tag;
  if (Settings().isAlwaysSaveFits())
    fn += ".fts";
  else
    fn += "." + info.suffix();
  QDir dir(outputpath);
  if (!dir.exists()) dir.mkpath(outputpath);
  fn = dir.absoluteFilePath(fn);
  if (!image->save(fn)) return ERROR;
  return OK;
}

void OpPlugin::log(const QString &msg)
{
  emit logOperation("",msg);
}

void OpPlugin::log(FitsImage* image, const QString &msg)
{
  if (image)
  {
    image->log(msg);
    emit logOperation(image->getName(),msg);
  }
  else
    emit logOperation("",msg);
}

void OpPlugin::log(std::shared_ptr<FitsObject> image, const QString &msg)
{
  if (image) log(image->getImage(),msg);
}

void OpPlugin::logProfiler(const QString& image, const QString& msg)
{
  if (profiler.getDuration() > 0)
  {
    QString name = QString::fromStdString(profiler.getName());
    emit logProfilerResult(name,image,0,0,profiler.getDuration(),msg);
  }
}

void OpPlugin::logProfiler(FitsImage* image, const QString& msg)
{
  if (profiler.getDuration() > 0)
  {
    QString name = QString::fromStdString(profiler.getName());
    emit logProfilerResult(name,image->getName(),image->getWidth(),image->getHeight(),profiler.getDuration(),msg);
//    qInfo(LOG_PROFILER(),"%s",profiler.toString().c_str());
  }
}


void OpPlugin::logProfiler(std::shared_ptr<FitsObject> image, const QString& msg)
{
  logProfiler(image->getImage(),msg);
}


