/********************************************************************************
 *                                                                              *
 * FitsIP - rank images by sharpness                                            *
 *                                                                              *
 * modified: 2022-12-02                                                         *
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

#ifndef MEASURERANK_H
#define MEASURERANK_H

#include <fitsip/core/opplugin.h>
#include <fitsip/core/dialogs/imageselectdialog.h>
#include <QObject>
#include <vector>

struct RankEntry
{
  QFileInfo info;
  double result;
};

/**
 * @brief This plugin ranks images by sharpness.
 *
 * The sharpness value is determines by applying the Laplacian kernel to the
 * image and than calculating the overall standard deviation.
 */
class MeasureRank: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  MeasureRank();
  virtual ~MeasureRank() override;

  virtual QString getMenuEntry() const override;

  virtual bool requiresFileList() const override;

  virtual ResultType execute(const std::vector<QFileInfo>& list, QRect selection=QRect(), const PreviewOptions& opt=PreviewOptions()) override;

private:
  RankEntry evaluate(const QFileInfo info, QRect selection);

  void copyToLog();

  std::vector<RankEntry> results;

};

#endif // MEASURERANK_H
