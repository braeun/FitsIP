/********************************************************************************
 *                                                                              *
 * FitsIP - dialog showing an informational text                                *
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

#ifndef TEXTINFODIALOG_H
#define TEXTINFODIALOG_H

#include <QDialog>

namespace Ui {
class TextInfoDialog;
}

class TextInfoDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TextInfoDialog(QWidget *parent = nullptr);
  ~TextInfoDialog();

  void setTitle(const QString& title);

  void setText(const QString& text);

private:
  Ui::TextInfoDialog *ui;
};

#endif // TEXTINFODIALOG_H
