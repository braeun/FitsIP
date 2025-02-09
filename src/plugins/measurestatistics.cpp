/********************************************************************************
 *                                                                              *
 * Fits - image statistics plugin                                               *
 *                                                                              *
 * modified: 2021-12-28                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) 2019-2021 by Harald Braeuning.   All Rights Reserved.          *
 ********************************************************************************
 * This program is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU General Public License                  *
 * as published by the Free Software Foundation; either version 3               *
 * of the License, or (at your option) any later version.                       *
 *                                                                              *
 * This program is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * General Public License for more details.                                     *
 *                                                                              *
 * You should have received a copy of the GNU General Public                    *
 * License along with this library; if not, write to the Free Software          *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   *
 ********************************************************************************/

#include "measurestatistics.h"
#include <fitsbase/dialogs/textinfodialog.h>
#include <limits>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <QByteArray>
#include <QTextStream>

MeasureStatistics::MeasureStatistics(bool inter):OpPlugin(inter)
{
  profiler = SimpleProfiler("MeasureStatistics");
}

MeasureStatistics::~MeasureStatistics()
{
}

QString MeasureStatistics::getMenuEntry() const
{
  return "Measure/Image Statistics...";
}

OpPlugin::ResultType MeasureStatistics::execute(std::shared_ptr<FitsObject> image, QRect rect, const PreviewOptions& opt)
{
  stat = ImageStatistics(*image->getImage(),rect);
  logOperation(image->getImage()->getName(),"Image Statistics:\n"+toString());
  if (interactive)
  {
    TextInfoDialog d;
    d.setTitle("Image Statistics");
    d.setText(toString());
    d.exec();
  }
  return OK;
}

const ImageStatistics& MeasureStatistics::getStatistics() const
{
  return stat;
}

QString MeasureStatistics::toString() const
{
  QByteArray a;
  QTextStream s(&a);
  s << "Statistics\n\n";
  const LayerStatistics& lstat = stat.getGlobalStatistics();
  s << "Summary:\n";
  s << "  # layers     : " << stat.getLayerStatistics().size() << "\n";
  s << "  area of interest: " << stat.getAOI().x() << "," << stat.getAOI().y() << " " << stat.getAOI().width() << "x" << stat.getAOI().height() << "\n\n";
  s << "  min value    : " << lstat.minValue << "\n";
  s << "  max value    : " << lstat.maxValue << "\n";
  s << "  mean value   : " << lstat.meanValue << "\n";
  s << "  standard dev.: " << lstat.stddev << "\n\n";
  if (stat.getLayerStatistics().size() > 1)
  {
    for (size_t index=0;index<stat.getLayerStatistics().size();index++)
    {
      const LayerStatistics& lstat = stat.getLayerStatistics()[index];
      s << "Layer: " << (index+1) << "\n";
      s << "  min value    : " << lstat.minValue << "\n";
      s << "  max value    : " << lstat.maxValue << "\n";
      s << "  mean value   : " << lstat.meanValue << "\n";
      s << "  standard dev.: " << lstat.stddev << "\n\n";
    }
  }
  s.flush();
  return a;
}

