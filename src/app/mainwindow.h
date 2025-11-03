/********************************************************************************
 *                                                                              *
 * FitsIP - main application window                                             *
 *                                                                              *
 * modified: 2025-11-01                                                         *
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

#include "widgets/imagewidget.h"
#include "script.h"
#include "scriptinterface.h"
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/imagecollection.h>
#include <fitsip/core/logbook/logbook.h>
#include <fitsip/core/pluginfactory.h>
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
class OpPluginData;
class EditMetadataDialog;
class FileList;
class FileListWidget;
class FileSystemView;
class HistogramView;
class HistoryTableWidget;
class LogbookWidget;
class LogWidget;
class MetadataTableWidget;
class PSFManagerDialog;
class PixelListWidget;
class ProfilerWidget;
class ProfileView;
class StarListWidget;
class XYChartsWidget;

struct PluginMenuEntry
{
  QAction* action;
  OpPlugin* plugin;
};

class MainWindow : public QMainWindow, public ScriptInterface
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  /**
   * @brief Initialization after window is shown.
   */
  void initialize(PluginFactory* factory);

  LogWidget* getLogWidget();

  virtual void setWorkingDir(const std::string& dir) override;

  virtual std::string getWorkingDir() const override;

  virtual std::shared_ptr<FitsObject> get(const std::string& filename) override;

  virtual const std::vector<std::shared_ptr<FitsObject>>& getOpen() const override;

  virtual std::shared_ptr<FitsObject> load(const std::string& filename) override;

  virtual bool save(std::shared_ptr<FitsObject> obj, const std::string& filename) override;

  virtual void display(std::shared_ptr<FitsObject> obj) override;

  virtual FileList* getSelectedFileList() const override;

protected:

  void closeEvent(QCloseEvent* event) override;

private slots:
  void logPluginOperation(QString image, QString op);

  void zoom(int z);

  void addPixel(QPoint p);

  void updateCursor(QPoint p);

  void updateAOI(QRect r);

  void showImageContextMenu(QPoint mouse, QPoint pixel);

  void on_actionMetadata_triggered();

  void onImageScaleChanged(double min, double max, int scale);

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

  void on_actionSave_As_PSF_triggered();

  void on_actionPSF_Manager_triggered();

  void on_actionPlugins_triggered();

private:

  enum SelectionMode { None, SelectPixel };

  void loadPlugins();
  void addOpPlugin(OpPlugin* op);
  QAction* addMenuEntry(QString entry, QIcon icon);
  void executeOpPlugin(OpPlugin* op);
  void executeOpPlugin(OpPlugin *op, std::shared_ptr<FitsObject> img, const OpPluginData& data);
  std::vector<QFileInfo> getFileList();
//  void display(int id);
  void updateDisplay();
  void updateMetadata();
  void run(const QFileInfo& fileinfo);
  void open(const QFileInfo& fileinfo);
  void openExternal(const QFileInfo& fileinfo);
  void copyImage();
  void openImage(const QFileInfo& fileinfo);
  void openSelection();
  void copySelectionToList();
  void fileListDoubleClicked(int index);
  void fileListOpenSelected();
  void openLogbook(const QString& name);
  void runScriptCmd(const QString& cmd);
  void runScriptFile(const QFileInfo& fileinfo);
  void setScriptOutput();
  void getStarlistFromPixellist();
  void toggleXYChartDisplay(bool flag);
  void annotate(QPoint pixel);

  Ui::MainWindow *ui;
  QMenu* openFileListMenu;
  QMenu* imageContextMenu;
  QPoint imageContextMenuAnchor; /* position of the context menu in image coordinates */
  std::unique_ptr<FileList> selectedFileList;
  std::unique_ptr<ImageCollection> imageCollection;
  PluginFactory* pluginFactory;
  std::vector<PluginMenuEntry> pluginMenus;
  std::map<QString,QToolBar*> pluginToolbars;
  std::unique_ptr<PixelList> defaultPixelList;
  std::unique_ptr<StarList> defaultStarList;
  ImageWidget* imageWidget;
  FileSystemView* filesystemView;
  FileListWidget* filelistWidget;
  HistogramView* histogramWidget;
  HistoryTableWidget* historyTableWidget;
  LogbookWidget* logbookWidget;
  LogWidget* logWidget;
  MetadataTableWidget* metaTableWidget;
  PixelListWidget* pixellistWidget;
  ProfilerWidget* profilerWidget;
  ProfileView* profileWidget;
  StarListWidget* starlistWidget;
  XYChartsWidget* chartsWidget;
  ConsoleWidget* consoleWidget;
  EditMetadataDialog* editMetadataDialog;
  Logbook logbook;
  SelectionMode selectionMode;
  PSFManagerDialog* psfManager;
  std::unique_ptr<Script> script;
  QMetaObject::Connection scriptOutConnection;
  QMetaObject::Connection scriptErrConnection;
};

#endif // MAINWINDOW_H
