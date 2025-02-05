/********************************************************************************
 *                                                                              *
 * FitsIP - widget with the general logging console                             *
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

#include "logwidget.h"
#include "ui_logwidget.h"
#include "appsettings.h"
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QMenu>
#include <QTextStream>

LogWidget::LogWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::LogWidget)
{
  ui->setupUi(this);
  contextMenu = new QMenu;
  contextMenu->addAction(ui->actionSave);
  contextMenu->addSeparator();
  contextMenu->addAction(ui->actionSelect_All);
  contextMenu->addAction(ui->actionCopy);
  contextMenu->addSeparator();
  contextMenu->addAction(ui->actionClear);
}

LogWidget::~LogWidget()
{
  delete ui;
}

void LogWidget::add(const QtMsgType type, const QMessageLogContext &context, const QString &message)
{
  switch (type)
  {
    case QtMsgType::QtInfoMsg:
      ui->logBrowser->append("<b>"+message+"</b>");
      break;
    case QtMsgType::QtWarningMsg:
      ui->logBrowser->append("<b><span style=\"color:darkorange\">"+message+"</span></b>");
      break;
    case QtMsgType::QtCriticalMsg:
      ui->logBrowser->append("<b><span style=\"color:red\">"+message+"</span></b>");
      break;
    default:
      ui->logBrowser->append(message);
      break;
  }
}

void LogWidget::writeStdOut(const QString& s)
{
  ui->logBrowser->insertPlainText(s);
}

void LogWidget::writeStdErr(const QString& s)
{
  ui->logBrowser->insertHtml("<b><span style=\"color:red\">"+s+"</span></b>");
}

void LogWidget::dockLocationChanged(Qt::DockWidgetArea area)
{
}


void LogWidget::on_actionSave_triggered()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,"Plain Text (*.txt);;HTML (*.html)");
  if (!fn.isNull())
  {
    QFile file(fn);
    if (file.open(QFile::WriteOnly))
    {
      if (fn.endsWith("html"))
      {
        file.write(ui->logBrowser->toHtml().toUtf8());
      }
      else
      {
        file.write(ui->logBrowser->toPlainText().toUtf8());
      }
    }
    file.close();
  }
}

void LogWidget::on_logBrowser_customContextMenuRequested(const QPoint &pos)
{
  contextMenu->popup(ui->logBrowser->mapToGlobal(pos));
}

