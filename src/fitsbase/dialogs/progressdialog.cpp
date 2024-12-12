/********************************************************************************
 *                                                                              *
 * FitsIP - dialog showing the progress of an operation                         *
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

#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QDebug>

ProgressDialog::ProgressDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::ProgressDialog),
  cancelled(false)
{
  ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
  delete ui;
}

void ProgressDialog::setTitle(const QString& title)
{
  ui->titleLabel->setText(title);
}

void ProgressDialog::appendMessage(const QString &txt)
{
  ui->textBrowser->append(txt);
}

void ProgressDialog::setMaximum(int max)
{
  ui->progressBar->setMaximum(max);
}

void ProgressDialog::setProgress(int value)
{
  ui->progressBar->setValue(value);
}

bool ProgressDialog::isCancelled() const
{
  return cancelled;
}



void ProgressDialog::reject()
{
  cancelled = true;
}

