/********************************************************************************
 *                                                                              *
 * FitsIP - measure the sharpness of images - result dialog                     *
 *                                                                              *
 * modified: 2025-02-10                                                         *
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

#ifndef MEASURESHARPNESSRESULTDIALOG_H
#define MEASURESHARPNESSRESULTDIALOG_H

#include <QDialog>
#include <vector>

class QFileInfo;
class SharpnessData;


namespace Ui {
class MeasureSharpnessResultDialog;
}

class MeasureSharpnessResultDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MeasureSharpnessResultDialog(QWidget *parent = nullptr);
  ~MeasureSharpnessResultDialog();

  void setResult(const std::vector<SharpnessData>& entries);

  std::vector<QFileInfo> getFileList() const;

signals:
  void writeToLogbook();
  void fileListChanged();

private slots:
  void on_removeRowsButton_clicked();

  void on_saveButton_clicked();

  void on_logButton_clicked();

private:
  Ui::MeasureSharpnessResultDialog *ui;
  std::vector<SharpnessData> entries;
};

#endif // MEASURESHARPNESSRESULTDIALOG_H
