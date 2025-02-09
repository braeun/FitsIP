﻿/********************************************************************************
 *                                                                              *
 * FitsIP - main application window                                             *
 *                                                                              *
 * modified: 2025-01-30                                                         *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imagewidget.h"
#include <fitsbase/fitsobject.h>
#include <fitsbase/logbook/logbook.h>
#include <QMainWindow>
#include <QMenu>
#include <vector>
#include <map>
#include <memory>

namespace Ui {
class MainWindow;
}

class ConsoleWidget;
class QListWidgetItem;
class QFileInfo;
class Plugin;
class OpPlugin;
class EditMetadataDialog;
class FileList;
class LogWidget;

class PreviewOptions;

struct PluginMenuEntry
{
  QAction* action;
  OpPlugin* plugin;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  /**
   * @brief Initialization after window is shown.
   */
  void initialize();

  LogWidget* getLogWidget();

protected:

  void closeEvent(QCloseEvent* event) override;

private slots:
  void logPluginOperation(QString image, QString op);

  void zoom(int32_t z);

  void addPixel(QPoint p);

  void updateCursor(QPoint p);

  void updateAOI(QRect r);

  void on_actionMetadata_triggered();

  void onImageScaleChanged(double min, double max, int32_t scale);

  void on_actionOpen_triggered();

  void on_actionQuit_triggered();

  void on_actionAbout_triggered();

  void on_actionAbout_Qt_triggered();

  void on_actionSave_triggered();

  void on_actionSave_As_triggered();

  void on_actionLoad_Plugin_triggered();

  void on_actionUndo_triggered();

  void on_openFileList_clicked(const QModelIndex &index);

  void on_openFileList_customContextMenuRequested(const QPoint &pos);

  void on_actionClose_Image_triggered();

  void on_actionClose_All_Images_triggered();

  void on_actionPreferences_triggered();

  void on_actionOpen_Logbook_triggered();

  void on_actionShow_Selected_Pixels_toggled(bool flag);

  void on_actionShow_Detected_Stars_toggled(bool flag);

  void on_actionNew_Logbook_triggered();

  void on_actionExport_Image_triggered();

  void on_actionNext_Image_triggered();

  void on_actionPrevious_Image_triggered();

  void on_actionAdd_Current_Image_to_List_triggered();

  void on_actionExport_Logbook_triggered();

  void on_actionClose_Logbook_triggered();

  void on_actionProperties_triggered();

  void on_actionClear_AOI_triggered();

  void on_actionSelect_Pixel_toggled(bool arg1);

  void on_actionRun_Script_triggered();

private:

  enum SelectionMode { None, SelectPixel };

  void loadPlugins();
  void addOpPlugin(OpPlugin* op);
  QAction* addMenuEntry(QString entry, QIcon icon);
  void executeOpPlugin(OpPlugin* op);
  void executeOpPlugin(OpPlugin *op, std::shared_ptr<FitsObject> img, QRect roi, const PreviewOptions& opt);
  std::vector<QFileInfo> getFileList();
  void display(std::shared_ptr<FitsObject> obj);
  void display(int id);
  void updateDisplay();
  void updateMetadata();
  void open(const QFileInfo& fileinfo);
  void openImage(const QFileInfo& fileinfo);
  void openSelection();
  void copySelectionToList();
  void fileListDoubleClicked(int index);
  void fileListOpenSelected();
  void openLogbook(const QString& name);
  void runScriptCmd(const QString& cmd);
  void runScriptFile(const QFileInfo& fileinfo);
  void setScriptOutput();

  Ui::MainWindow *ui;
  QMenu* openFileListMenu;
  std::vector<PluginMenuEntry> pluginMenus;
  std::map<QString,QToolBar*> pluginToolbars;
  ImageWidget* imageWidget;
  ConsoleWidget* consoleWidget;
  EditMetadataDialog* editMetadataDialog;
  Logbook logbook;
  SelectionMode selectionMode;
  QMetaObject::Connection scriptOutConnection;
  QMetaObject::Connection scriptErrConnection;
};

#endif // MAINWINDOW_H
