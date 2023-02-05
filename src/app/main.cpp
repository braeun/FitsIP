/********************************************************************************
 *                                                                              *
 * FitsIP - main program entry                                                  *
 *                                                                              *
 * modified: 2022-11-20                                                         *
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

#include "mainwindow.h"
#include "logwidget.h"
#include "palettefactory.h"
#include "appsettings.h"
#include <QApplication>
#include <QDebug>
#include <QStyleFactory>

static MainWindow* mainwindow = nullptr;
static LogWidget* logwidget = nullptr;

static void msgHandler(const QtMsgType type, const QMessageLogContext& context, const QString &message)
{
  if (logwidget)
  {
    logwidget->add(type,context,message);
  }
  else
  {
    std::cout << message.toStdString() << std::endl;
  }
}

int main(int argc, char* argv[])
{
  QCoreApplication::setOrganizationName("hbr");
  QCoreApplication::setApplicationName("fits");
  QCoreApplication::setApplicationVersion(QString("%1.%2.%3").arg(FITS_VERSION_MAJOR).arg(FITS_VERSION_MINOR).arg(FITS_VERSION_PATCH));
  QApplication::setApplicationDisplayName("Fits");
  QApplication a(argc, argv);

  QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/icons" << ":/pluginicons/resources/icons");

  qInstallMessageHandler(msgHandler);

  PaletteFactory::registerDefaultPalette(QApplication::palette());

  AppSettings settings;
  QString style = settings.getStyle();
  QApplication::setStyle(QStyleFactory::create(style));
  QString palette = settings.getPalette();
  if (!palette.isEmpty()) QApplication::setPalette(PaletteFactory::getPalette(palette));

  mainwindow = new MainWindow;
  mainwindow->setWindowTitle("");
  mainwindow->showMaximized();
  mainwindow->initialize();
  logwidget = mainwindow->getLogWidget();
  int32_t ret = a.exec();
  qInstallMessageHandler(nullptr);
  delete  mainwindow;
  return ret;
}
