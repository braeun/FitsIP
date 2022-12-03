/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for the result of the ranking by sharpness                   *
 *                                                                              *
 * modified: 2022-12-01                                                         *
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

#ifndef MEASURERANKRESULTDIALOG_H
#define MEASURERANKRESULTDIALOG_H

#include <QDialog>
#include <vector>

class QFileInfo;
class RankEntry;

namespace Ui {
class MeasureRankResultDialog;
}

class MeasureRankResultDialog : public QDialog
{
  Q_OBJECT
public:
  explicit MeasureRankResultDialog(QWidget *parent = nullptr);
  ~MeasureRankResultDialog();

  void setResult(const std::vector<RankEntry>& entries);

  const std::vector<QFileInfo>& getFileList() const;

signals:
  void writeToLogbook();

private slots:
  void on_copyButton_clicked();

  void on_removeRowsButton_clicked();

  void on_saveButton_clicked();

  void on_logButton_clicked();

private:
  Ui::MeasureRankResultDialog *ui;
  std::vector<RankEntry> entries;
  std::vector<QFileInfo> filelist;
};

#endif // MEASURERANKRESULTDIALOG_H
