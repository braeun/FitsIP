/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to set the log book filter                                   *
 *                                                                              *
 * modified: 2022-11-30                                                         *
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

#include "logbookfilter1dialog.h"
#include "ui_logbookfilter1dialog.h"
#include <fitsbase/logbook/logbook.h>
#include <fitsbase/logbook/logbookfilter.h>

LogbookFilter1Dialog::LogbookFilter1Dialog(const Logbook* logbook, QWidget *parent):QDialog(parent),
  ui(new Ui::LogbookFilter1Dialog),
  logbook(logbook)
{
  ui->setupUi(this);
  QDateTime begin,end;
  logbook->getTimeRange(&begin,&end);
  ui->fromField->setDate(begin.date());
  ui->toField->setDate(end.date());
  QStringList list;
  list.push_back("");
  for (const QString& p : logbook->getProjects()) list.push_back(p);
  ui->projectBox->addItems(list);
  ui->projectBox->setCurrentIndex(0);
}

LogbookFilter1Dialog::~LogbookFilter1Dialog()
{
  delete ui;
}

LogbookFilter LogbookFilter1Dialog::getFilter() const
{
  LogbookFilter filter;
  filter.setProject(ui->projectBox->currentText());
  filter.setStep(ui->stepBox->currentText());
  filter.setFrom(ui->fromField->dateTime());
  filter.setTo(ui->toField->dateTime().addDays(1));
  return filter;
}

void LogbookFilter1Dialog::on_projectBox_currentTextChanged(const QString &text)
{
  QStringList list;
  list.push_back("");
  for (const QString& p : logbook->getSteps(text)) list.push_back(p);
  ui->stepBox->clear();
  ui->stepBox->addItems(list);
  ui->stepBox->setCurrentIndex(0);
}
