/********************************************************************************
 *                                                                              *
 * Fits - image statistics plugin                                               *
 *                                                                              *
 * modified: 2021-12-27                                                         *
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

#ifndef MEASURESTATISTICS_H
#define MEASURESTATISTICS_H

#include <fitsbase/imagestatistics.h>
#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class MeasureStatistics: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
  Q_INTERFACES(OpPlugin)

public:
  MeasureStatistics(bool interactive=true);
  virtual ~MeasureStatistics();

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsImage> image, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

  const ImageStatistics& getStatistics() const;

  QString toString() const;

private:
  ImageStatistics stat;
};

#endif // MEASURESTATISTICS_H
