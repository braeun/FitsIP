/********************************************************************************
 *                                                                              *
 * FitsIP - point spread function parameter widget                              *
 *                                                                              *
 * modified: 2025-05-30                                                         *
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

#ifndef PSFWIDGET_H
#define PSFWIDGET_H

#include "../fitstypes.h"
#include <QWidget>

namespace Ui {
class PSFWidget;
}

class PSFWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PSFWidget(QWidget *parent = nullptr);
  ~PSFWidget();

  void updatePSFList();

  QString getFunction() const;

  std::vector<ValueType> getParameters() const;

signals:
  void functionSelected(QString name);

private:
  void selectionChanged(const QString &text);

  Ui::PSFWidget *ui;
};

#endif // PSFWIDGET_H
