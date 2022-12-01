/********************************************************************************
 *                                                                              *
 * FitsIP - rank images by sharpness                                            *
 *                                                                              *
 * modified: 2022-11-21                                                         *
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

#include "measurerank.h"
#include "measurerankresultdialog.h"
#include "measurestatistics.h"
#include "opkernel.h"
#include "kernelrepository.h"
#include <fitsimage.h>
#include <dialogs/progressdialog.h>
#include <io/iofactory.h>
#include <algorithm>
#include <QApplication>


MeasureRank::MeasureRank()
{
  profiler = SimpleProfiler("MeasureRank");
}

MeasureRank::~MeasureRank()
{
}

QString MeasureRank::getMenuEntry() const
{
  return "Measure/Rank Images...";
}

bool MeasureRank::requiresFileList() const
{
  return true;
}

OpPlugin::ResultType MeasureRank::execute(const std::vector<QFileInfo>& list, QRect selection)
{
  if (list.empty()) return CANCELLED;
  ProgressDialog* prog = list.size() > 2 ? new ProgressDialog() : nullptr;
  if (prog)
  {
    prog->setMaximum(list.size());
    prog->show();
  }
  results.clear();
  int32_t n = 0;
  for (const QFileInfo& info : list)
  {
    if (prog)
    {
      prog->setProgress(n++);
      prog->appendMessage(info.fileName());
      QApplication::processEvents();
      if (prog->isCancelled()) break;
    }
    RankEntry entry = evaluate(info,selection);
    if (entry.info.exists()) results.push_back(entry);
  }
  if (prog) prog->deleteLater();
  if (results.empty()) return CANCELLED;
  std::sort(results.begin(),results.end(),[](const RankEntry& e1, const RankEntry& e2){return e1.result>e2.result;});
  if (interactive)
  {
    MeasureRankResultDialog d;
    d.setResult(results);
    d.exec();
    filelist = d.getFileList();
  }
  return OK;
}


RankEntry MeasureRank::evaluate(const QFileInfo info, QRect selection)
{
  IOHandler* handler = IOFactory::getInstance()->getHandler(info.absoluteFilePath());
  if (!handler) return RankEntry();
  try
  {
    auto img = handler->read(info.absoluteFilePath());
    /* apply laplacian */
    OpKernel op;
    const Kernel& kernel = KernelRepository::instance().getKernel(KernelRepository::LAPLACIAN);
    op.convolve(img,kernel);
    /* ignore borders */
    QRect r(5,5,img->getWidth()-10,img->getHeight()-10);
    img = img->subImage(r);
    /* calculate statistics */
    MeasureStatistics s(false);
    s.execute(img,selection);
    RankEntry entry;
    entry.info = info;
    entry.result = s.getStatistics().getGlobalStatistics().stddev;
    return entry;
  }
  catch (std::exception& ex)
  {
  }
  return RankEntry();
}

