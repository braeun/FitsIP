/********************************************************************************
 *                                                                              *
 * FitsIP - main application window                                             *
 *                                                                              *
 * modified: 2025-11-08                                                         *
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
#include "ui_mainwindow.h"
#include "appsettings.h"
#ifdef USE_PYTHON
#include <pythonscript.h>
#endif
#include "script.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/cameradatabasedialog.h"
#include "dialogs/configurationdialog.h"
#include "dialogs/editmetadatadialog.h"
#include "dialogs/logbookexportdialog.h"
#include "dialogs/logbookpropertiesdialog.h"
#include "dialogs/sysinfodialog.h"
#include "widgets/consolewidget.h"
#include "widgets/filelistwidget.h"
#include "widgets/filesystemview.h"
#include "widgets/histogramview.h"
#include "widgets/historytablewidget.h"
#include "widgets/logbookwidget.h"
#include "widgets/logwidget.h"
#include "widgets/metadatatablewidget.h"
#include "widgets/pixellistwidget.h"
#include "widgets/profilertablemodel.h"
#include "widgets/profilerwidget.h"
#include "widgets/profileview.h"
#include "widgets/starlistwidget.h"
#include <fitsip/core/externaltoolslauncher.h>
#include <fitsip/core/filelist.h>
#include <fitsip/core/pixellist.h>
#include <fitsip/core/dialogs/pluginfilelistreturndialog.h>
#include <fitsip/core/dialogs/plugininfodialog.h>
#include <fitsip/core/dialogs/twovaluedialog.h>
#include <fitsip/core/io/iofactory.h>
#include <fitsip/core/logbook/logbookutils.h>
#include <fitsip/core/opplugin.h>
#include <fitsip/core/psf/psffactory.h>
#include "fitsip/core/psf/psfmanagerdialog.h"
#include <fitsip/core/pluginfactory.h>
#include <fitsip/core/widgets/previewoptions.h>
#include <fitsip/core/widgets/xychartswidget.h>
#include <QActionGroup>
#include <QCloseEvent>
#include <QDir>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QProcess>
#include <QSettings>
#include <QThread>
#include <QDebug>

static const char* last_menu = "Window";

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),
  ui(new Ui::MainWindow),
  editMetadataDialog(nullptr),
  selectionMode(None),
  psfManager(nullptr),
  sysinfoDialog(nullptr)
{
  ui->setupUi(this);
  AppSettings settings;
//  db::configure(settings);

  openFileListMenu = new QMenu;
  openFileListMenu->addAction("Copy",this,[=](){copyImage();});
  openFileListMenu->addSeparator();
  openFileListMenu->addAction("Close",this,[=](){on_actionClose_Image_triggered();});

  imageContextMenu = new QMenu;
  imageContextMenu->addAction("Add Pixel",this,[=](){addPixel(imageContextMenuAnchor);});
  imageContextMenu->addSeparator();
  imageContextMenu->addAction("Annotate...",this,[=](){annotate(imageContextMenuAnchor);});

  QDockWidget *consoleDockWidget = new QDockWidget(tr("Script Console"), this);
  consoleDockWidget->setObjectName("console");
  consoleDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
  consoleWidget = new ConsoleWidget(consoleDockWidget);
  addDockWidget(Qt::BottomDockWidgetArea,consoleDockWidget);
  consoleDockWidget->setWidget(consoleWidget);

  logWidget = new LogWidget();
  ui->logDockContents->layout()->addWidget(logWidget);
  logbookWidget = new LogbookWidget();
  ui->logbookDockContents->layout()->addWidget(logbookWidget);
  histogramWidget = new HistogramView();
  ui->histogramDockContents->layout()->addWidget(histogramWidget);
  profileWidget = new ProfileView();
  ui->profilesDockContents->layout()->addWidget(profileWidget);
  profilerWidget = new ProfilerWidget();
  ui->profilerDockContents->layout()->addWidget(profilerWidget);
  pixellistWidget = new PixelListWidget();
  ui->pixellistDockContents->layout()->addWidget(pixellistWidget);
  starlistWidget = new StarListWidget();
  ui->starlistDockContents->layout()->addWidget(starlistWidget);
  filesystemView = new FileSystemView();
  ui->fileSystemDockContents->layout()->addWidget(filesystemView);
  filelistWidget = new FileListWidget();
  ui->fileListDockContents->layout()->addWidget(filelistWidget);
  metaTableWidget = new MetadataTableWidget();
  ui->metadataDockContents->layout()->addWidget(metaTableWidget);
  historyTableWidget = new HistoryTableWidget();
  ui->historyDockContents->layout()->addWidget(historyTableWidget);

  imageWidget = new ImageWidget();
  connect(imageWidget,&ImageWidget::cursorSet,this,&MainWindow::setCursor);
  connect(imageWidget,&ImageWidget::contextMenuRequested,this,&MainWindow::showImageContextMenu);
  connect(imageWidget,&ImageWidget::cursorMoved,this,&MainWindow::updateCursor);
  connect(imageWidget,&ImageWidget::cursorMoved,profileWidget,&ProfileView::updateCursor);
  connect(imageWidget,&ImageWidget::cursorSet,profileWidget,&ProfileView::setCursor);
  connect(imageWidget,&ImageWidget::aoiChanged,this,&MainWindow::updateAOI);
  ui->scrollArea->setWidget(imageWidget);
  ui->scrollArea->setWidgetResizable(true);
  ui->scrollArea->setBackgroundRole(QPalette::Shadow);
  connect(histogramWidget,&HistogramView::imageScaleChanged,this,&MainWindow::onImageScaleChanged);

  chartsWidget = new XYChartsWidget();
  ui->chartPageLayout->addWidget(chartsWidget);

  connect(filesystemView,&FileSystemView::openFile,this,[this](QString file){open(QFileInfo(file));});
  connect(filesystemView,&FileSystemView::runFile,this,[this](QString file){run(QFileInfo(file));});
  connect(filesystemView,&FileSystemView::openSelection,this,[this]{openSelection();});
  connect(filesystemView,&FileSystemView::copySelectionToFilelist,this,[this]{copySelectionToList();});
  connect(filesystemView,&FileSystemView::workingDirChanged,filelistWidget,&FileListWidget::setWorkingDir);

  connect(filelistWidget,&FileListWidget::openSelected,this,&MainWindow::fileListOpenSelected);


  connect(ui->logDockWidget,&QDockWidget::dockLocationChanged,logWidget,&LogWidget::dockLocationChanged);
  connect(ui->logbookDockWidget,&QDockWidget::dockLocationChanged,logbookWidget,&LogbookWidget::dockLocationChanged);

  connect(ui->actionFit_Window,&QAction::triggered,this,[=](){ zoom(0); });
  connect(ui->actionOriginal_Size,&QAction::triggered,this,[=](){ zoom(1); });
  connect(ui->action1_2,&QAction::triggered,this,[=](){ zoom(-2); });
  connect(ui->action1_4,&QAction::triggered,this,[=](){ zoom(-4); });
  connect(ui->action2_1,&QAction::triggered,this,[=](){ zoom(2); });
  connect(ui->action4_1,&QAction::triggered,this,[=](){ zoom(4); });

  connect(ui->actionClear_File_List,&QAction::triggered,filelistWidget,&FileListWidget::clear);
  connect(ui->actionClear_Pixel_List,&QAction::triggered,pixellistWidget,&PixelListWidget::clear);
  connect(ui->actionClear_Star_List,&QAction::triggered,starlistWidget,&StarListWidget::clear);

  connect(ui->actionShow_XY_Charts,&QAction::toggled,this,&MainWindow::toggleXYChartDisplay);

  connect(ui->actionCameras,&QAction::triggered,this,[](){CameraDatabaseDialog::showDialog();});

  connect(ui->actionSystem_Information,&QAction::triggered,this,&MainWindow::showSysInfo);

  ui->menuWindow->addAction(ui->fileSystemDockWidget->toggleViewAction());
  ui->fileSystemDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F1));
  ui->menuWindow->addAction(ui->fileListDockWidget->toggleViewAction());
  ui->fileListDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F2));
  ui->menuWindow->addAction(ui->openFilesDockWidget->toggleViewAction());
  ui->openFilesDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F3));
  ui->menuWindow->addSeparator();
  ui->menuWindow->addAction(ui->metadataDockWidget->toggleViewAction());
  ui->metadataDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F4));
  ui->menuWindow->addAction(ui->historyDockWidget->toggleViewAction());
  ui->historyDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F5));
  ui->menuWindow->addAction(ui->histogramDockWidget->toggleViewAction());
  ui->histogramDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F6));
  ui->menuWindow->addAction(ui->profilesDockWidget->toggleViewAction());
  ui->profilesDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F7));
  ui->menuWindow->addAction(ui->starlistDockWidget->toggleViewAction());
  ui->starlistDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F8));
  ui->menuWindow->addAction(ui->pixellistDockWidget->toggleViewAction());
  ui->pixellistDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F9));
  ui->menuWindow->addSeparator();
  ui->menuWindow->addAction(ui->logbookDockWidget->toggleViewAction());
  ui->logbookDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F10));
  ui->menuWindow->addAction(ui->logDockWidget->toggleViewAction());
  ui->logDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F11));
  ui->menuWindow->addAction(ui->profilerDockWidget->toggleViewAction());
//  ui->profilerDockWidget->toggleViewAction()->setShortcut(QKeySequence());
  ui->menuWindow->addAction(consoleDockWidget->toggleViewAction());
  consoleDockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_F12));

  ui->fileListDockWidget->close();
  ui->profilesDockWidget->close();
  ui->starlistDockWidget->close();
  ui->pixellistDockWidget->close();
  ui->historyDockWidget->close();
//  consoleDockWidget->close();

  {
    QSettings settings;
    if (settings.contains(AppSettings::MAINWINDOW_STATE))
    {
      restoreState(settings.value(AppSettings::MAINWINDOW_STATE).toByteArray());
    }
    if (settings.contains(AppSettings::MAINWINDOW_GEOMETRY))
    {
      restoreGeometry(settings.value(AppSettings::MAINWINDOW_GEOMETRY).toByteArray());
    }
  }

  if (!settings.getLogbook().isEmpty() && settings.isLogbookOpenLast()) openLogbook(settings.getLogbook());
  logbookWidget->setLogbook(&logbook);

  connect(ui->actionActive,&QAction::toggled,&logbook,&Logbook::activate);

  connect(ui->actionLoad_File_List,&QAction::triggered,filelistWidget,&FileListWidget::load);
  connect(ui->actionAppend_File_List,&QAction::triggered,filelistWidget,&FileListWidget::append);
  connect(ui->actionSave_File_List,&QAction::triggered,filelistWidget,&FileListWidget::save);
  connect(ui->actionLoad_Pixel_List,&QAction::triggered,pixellistWidget,&PixelListWidget::load);
  connect(ui->actionSave_Pixel_List,&QAction::triggered,pixellistWidget,&PixelListWidget::save);
  connect(ui->actionLoad_Star_List,&QAction::triggered,starlistWidget,&StarListWidget::load);
  connect(ui->actionSave_Star_List,&QAction::triggered,starlistWidget,&StarListWidget::save);

  connect(IOFactory::getInstance(),&IOFactory::logProfilerResult,profilerWidget->getModel(),&ProfilerTableModel::addProfilerResult);

  QActionGroup* grp = new QActionGroup(this);
  ui->actionTable->setActionGroup(grp);
  ui->actionBy_Date->setActionGroup(grp);
  ui->actionTree_By_Project->setActionGroup(grp);
  ui->actionTree_By_Project_and_Date->setActionGroup(grp);
  ui->actionTree_By_Project_and_Step->setActionGroup(grp);
  connect(ui->actionTable,&QAction::triggered,this,[=](bool checked){if (checked) logbookWidget->setDisplay(LogbookWidget::Table);});
  connect(ui->actionBy_Date,&QAction::triggered,this,[=](bool checked){if (checked) logbookWidget->setDisplay(LogbookWidget::TreeByDate);});
  connect(ui->actionTree_By_Project,&QAction::triggered,this,[=](bool checked){if (checked) logbookWidget->setDisplay(LogbookWidget::TreeByProject);});
  connect(ui->actionTree_By_Project_and_Date,&QAction::triggered,this,[=](bool checked){if (checked) logbookWidget->setDisplay(LogbookWidget::TreeByProjectByDate);});
  connect(ui->actionTree_By_Project_and_Step,&QAction::triggered,this,[=](bool checked){if (checked) logbookWidget->setDisplay(LogbookWidget::TreeByProjectByStep);});
  switch (settings.getLogwidgetStyle())
  {
    case 0:
      ui->actionTable->setChecked(true);
      logbookWidget->setDisplay(LogbookWidget::Table);
      break;
    case 1:
      ui->actionBy_Date->setChecked(true);
      logbookWidget->setDisplay(LogbookWidget::TreeByDate);
      break;
    case 2:
      ui->actionTree_By_Project->setChecked(true);
      logbookWidget->setDisplay(LogbookWidget::TreeByProject);
      break;
    case 3:
      ui->actionTree_By_Project_and_Date->setChecked(true);
      logbookWidget->setDisplay(LogbookWidget::TreeByProjectByDate);
      break;
    case 4:
      ui->actionTree_By_Project_and_Step->setChecked(true);
      logbookWidget->setDisplay(LogbookWidget::TreeByProjectByStep);
      break;
  }
  ui->actionShow_Selected_Pixels->setChecked(settings.isShowPixellist());
  ui->actionShow_Detected_Stars->setChecked(settings.isShowStarlist());

  ui->actionRun_Script->setEnabled(false);

  filelistWidget->setWorkingDir(filesystemView->getRoot());
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue(AppSettings::MAINWINDOW_STATE,saveState());
  settings.setValue(AppSettings::MAINWINDOW_GEOMETRY,saveGeometry());
  delete ui;
}

void MainWindow::initialize(PluginFactory* factory)
{
  pluginFactory = factory;
  defaultPixelList = std::make_unique<PixelList>();
  pixellistWidget->setPixelList(defaultPixelList.get());
//  connect(ui->pixellistWidget,&PixelListWidget::findStars,this,&MainWindow::getStarlistFromPixellist);
  defaultStarList = std::make_unique<StarList>();
  starlistWidget->setStarList(defaultStarList.get());
  imageCollection = std::make_unique<ImageCollection>();
  //  scriptInterface = std::make_unique<ScriptInterface>();
  selectedFileList = std::make_unique<FileList>();
  filelistWidget->setFileList(selectedFileList.get());
  ui->openFileList->setModel(imageCollection.get());
  connect(pluginFactory,&PluginFactory::logOperation,this,&MainWindow::logPluginOperation);
  connect(pluginFactory,&PluginFactory::logProfilerResult,profilerWidget->getModel(),&ProfilerTableModel::addProfilerResult);
  loadPlugins();
#ifdef USE_PYTHON
  script = std::make_unique<PythonScript>(this,pluginFactory);
#endif
  if (script)
  {
    ui->actionRun_Script->setEnabled(true);
    connect(consoleWidget,&ConsoleWidget::consoleCommand,this,&MainWindow::runScriptCmd);
    setScriptOutput();
  }
}

LogWidget* MainWindow::getLogWidget()
{
  return logWidget;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (!imageCollection->isEmpty())
  {
    if (QMessageBox::question(this,QApplication::applicationDisplayName(),
                              "Really quit?\nUnsaved data will be lost!") != QMessageBox::Yes)
    {
      event->ignore();
      return;
    }
  }
  event->accept();
}

void MainWindow::loadPlugins()
{
  auto list = pluginFactory->getOpPlugins();
  for (OpPlugin* p : list)
  {
    addOpPlugin(p);
  }
}

void MainWindow::addOpPlugin(OpPlugin *op)
{
  if (op->getMenuEntry().isEmpty())
  {
    qWarning() << "No menu entry provided for plugin!";
    return;
  }
  QAction* a = addMenuEntry(op->getMenuEntry(),op->getIcon());
  if (a != nullptr)
  {
    PluginMenuEntry entry;
    entry.action = a;
    entry.plugin = op;
    pluginMenus.push_back(entry);
    connect(a,&QAction::triggered,this,[=](){this->executeOpPlugin(op);});
  }
}

QAction* MainWindow::addMenuEntry(QString entry, QIcon icon)
{
  QStringList entries = entry.split("/");
  if (entries.size() < 2)
  {
    qWarning() << "Illegal menu entry provided for plugin";
    return nullptr;
  }
  QStringList list = AppSettings().getMenuOrder();
  QMenu* menu = nullptr;
  QMenu* lastmenu = nullptr;
  QList<QAction*> actions = ui->menubar->actions();
  for (int i=0;i<entries.size()-1;i++)
  {
    for (QAction* a : actions)
    {
      if (a->text() == entries[i])
      {
        menu = a->menu();
        break;
      }
    }
    if (menu == nullptr)
    {
      menu = new QMenu(entries[i]);
      if (i == 0) /* add new toplevel menu */
      {
        int index = 0;
        while (index < list.size() && entries[i] != list[index]) ++index;
        for (QAction* a : actions)
        {
          int next = index + 1;
          while (next < list.size() && a->text() != list[next]) ++next;
          if (next < list.size() || a->text() == last_menu)
          {
            ui->menubar->insertMenu(a,menu);
            break;
          }
        }
      }
      else
      {
        bool inserted = false;
        for (QAction* a : actions)
        {
          if (a->text() > entries[i])
          {
            lastmenu->insertMenu(a,menu);
            inserted = true;
            break;
          }
        }
        if (!inserted)
        {
          lastmenu->addMenu(menu);
        }
      }
    }
    actions = menu->actions();
    lastmenu = menu;
    menu = nullptr;
  }
  if (!lastmenu) return nullptr;
  QAction* action = new QAction(entries.last());
  bool inserted = false;
  for (QAction* a : actions)
  {
    if (a->text() > entries.last())
    {
      lastmenu->insertAction(a,action);
      inserted = true;
      break;
    }
  }
  if (!inserted)
  {
    lastmenu->addAction(action);
  }
  if (!icon.isNull())
  {
    action->setIcon(icon);
    QToolBar* tb;
    if (pluginToolbars.find(entries[0]) == pluginToolbars.end())
    {
      tb = addToolBar(entries[0]);
      tb->setObjectName(entries[0]);
      pluginToolbars[entries[0]] = tb;
    }
    else
    {
      tb = pluginToolbars[entries[0]];
    }
    tb->addAction(action);
  }
  return action;
}

void MainWindow::executeOpPlugin(OpPlugin *op)
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  OpPluginData data;
  data.aoi = imageWidget->getAOI();
  data.previewOptions.scale = static_cast<FitsImage::Scale>(histogramWidget->getImageScale());
  data.imageCollection = imageCollection.get();
  data.pixellist = (activeFile) ? activeFile->getPixelList() : defaultPixelList.get();
  data.starlist = (activeFile) ? activeFile->getStarList() : defaultStarList.get();
  if (op->requiresFileList())
  {
    std::vector<std::shared_ptr<FitsObject>> imglist;
    for (const std::shared_ptr<FitsObject>& obj : imageCollection->getFiles())
    {
      imglist.push_back(obj);
    }
    std::vector<QFileInfo> filelist = getFileList();
    OpPlugin::ResultType ret;
    if (imglist.empty() && filelist.empty())
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Plugin requires a list of files!");
      return;
    }
    else if (filelist.empty())
    {
      ret = op->execute(imglist,data);
    }
    else
    {
      ret = op->execute(filelist,data);
    }
    if (ret == OpPlugin::OK)
    {
      if (!op->getFileList().empty() && selectedFileList)
      {
        PluginFilelistReturnDialog d(this);
        d.setListSize(op->getFileList().size());
        d.exec();
        switch (d.getAction())
        {
          case PluginFilelistReturnDialog::Ignore:
            break;
          case PluginFilelistReturnDialog::Append:
            selectedFileList->addFiles(op->getFileList());
            break;
          case PluginFilelistReturnDialog::Replace:
            selectedFileList->setFiles(op->getFileList());
            break;
        }
      }
      auto list = op->getCreatedImages();
      if (!list.empty())
      {
        for (auto img : list)
        {
//            std::shared_ptr<FitsObject> file = std::make_shared<FitsObject>("",img);
          imageCollection->addFile(img);
        }
        ui->openFileList->selectionModel()->clearSelection();
        ui->openFileList->selectionModel()->setCurrentIndex(imageCollection->index(imageCollection->rowCount()-1,0),QItemSelectionModel::SelectCurrent);
  //      ui->scrollArea->setWidgetResizable(false);
        imageCollection->setActiveFile(imageCollection->rowCount()-1);
        display(imageCollection->getActiveFile());
      }
    }
    else if (ret == OpPlugin::ERROR)
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Plugin execution error!\n"+op->getError());
    }
  }
  else if (activeFile)
  {
    qDebug() << "Executing: " << op->getMenuEntry();
    activeFile->pushUndo();
    ui->actionUndo->setEnabled(activeFile->isUndoAvailable());
    executeOpPlugin(op,activeFile,data);
  }
  else if (!op->requiresImage())
  {
    executeOpPlugin(op,std::shared_ptr<FitsObject>(),data);
  }
  else
  {
    QMessageBox::information(this,QApplication::applicationDisplayName(),"No image displayed!");
  }
}

std::vector<QFileInfo> MainWindow::getFileList()
{
  int src = -1;
  QStringList files;
  if (filesystemView->isVisible())
  {
    files = filesystemView->getSelectedFiles();
  }
  if (filelistWidget->isVisible() && selectedFileList && !selectedFileList->getFiles().empty())
  {
    src = 0;
  }
  else if (!files.isEmpty())
  {
    src = 1;
  }
  std::vector<QFileInfo> filelist;
  switch (src)
  {
    case 0:
      filelist = selectedFileList->getFiles();
      break;
    case 1:
      for (const QString& file : files)
      {
        filelist.push_back(QFileInfo(file));
      }
      break;
  }
  return filelist;
}

void MainWindow::executeOpPlugin(OpPlugin *op, std::shared_ptr<FitsObject> img, const OpPluginData& data)
{
  OpPlugin::ResultType ret = op->execute(img,data);
  if (ret == OpPlugin::OK)
  {
    auto list = op->getCreatedImages();
    if (!list.empty())
    {
      for (const auto& img : list)
      {
//        std::shared_ptr<FitsObject> file = std::make_shared<FitsObject>("",img);
        imageCollection->addFile(img);
      }
      ui->openFileList->selectionModel()->clearSelection();
      ui->openFileList->selectionModel()->setCurrentIndex(imageCollection->index(imageCollection->rowCount()-1,0),QItemSelectionModel::SelectCurrent);
//      ui->scrollArea->setWidgetResizable(false);
      imageCollection->setActiveFile(imageCollection->rowCount()-1);
      display(imageCollection->getActiveFile());
    }
    else
    {
      std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
      if (activeFile) activeFile->updateHistogram();
      updateDisplay();
    }
    if (!op->getFileList().empty() && selectedFileList)
    {
      PluginFilelistReturnDialog d(this);
      d.setListSize(op->getFileList().size());
      d.exec();
      switch (d.getAction())
      {
        case PluginFilelistReturnDialog::Ignore:
          break;
        case PluginFilelistReturnDialog::Append:
          selectedFileList->addFiles(op->getFileList());
          break;
        case PluginFilelistReturnDialog::Replace:
          selectedFileList->setFiles(op->getFileList());
          break;
      }
    }
  }
  else if (ret == OpPlugin::ERROR)
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Plugin execution error!\n"+op->getError());
  }
}

void MainWindow::display(std::shared_ptr<FitsObject> file)
{
  imageCollection->setActiveFile(file);
  if (file)
  {
    QFileInfo info(file->getFilename());
    ui->activeFileLabel->setText(info.fileName());
    pixellistWidget->setPixelList(file->getPixelList());
    starlistWidget->setStarList(file->getStarList());
  }
  else
  {
    ui->activeFileLabel->setText("");
  }
  updateDisplay();
//  updateOpenFileListSelection();
}

void MainWindow::updateDisplay()
{
  SimpleProfiler profiler("UpdateDisplay");
  profiler.start();
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  histogramWidget->setImage(activeFile);
  profileWidget->setImage(activeFile);
  if (activeFile)
  {
    double scaleMin = histogramWidget->getScaleMin();
    double scaleMax = histogramWidget->getScaleMax();
    FitsImage::Scale scale = static_cast<FitsImage::Scale>(histogramWidget->getImageScale());
    QImage tmp = activeFile->getImage().toQImage(scaleMin,scaleMax,scale);
    imageWidget->setImage(tmp,activeFile->getPixelList(),activeFile->getStarList());
    if (!ui->scrollArea->widgetResizable()) imageWidget->adjustSize();
    imageWidget->setAOI(activeFile->getAOI());
    QString txt = QString::asprintf("%4d,%4d",activeFile->getImage().getWidth(),activeFile->getImage().getHeight());
    switch (activeFile->getImage().getDepth())
    {
      case 1:
        txt += " GRAY";
        break;
      case 2:
        txt += " FFT";
        break;
      case 3:
        txt += " RGB";
        break;
    }
    ui->infoLabel->setText(txt);
  }
  else
  {
    imageWidget->setImage(QImage(),nullptr,nullptr);
  }
  updateMetadata();
  chartsWidget->setFitsObject(activeFile);
  profiler.stop();
  if (activeFile)
  {
    profilerWidget->getModel()->addProfilerResult(QString::fromStdString(profiler.getName()),
                                                      activeFile->getImage().getName(),
                                                      activeFile->getImage().getWidth(),
                                                      activeFile->getImage().getHeight(),
                                                      profiler.getDuration(),
                                                      "");
  }
}

void MainWindow::updateMetadata()
{
  metaTableWidget->clear();
  historyTableWidget->clear();
  if (imageCollection->getActiveFile())
  {
    metaTableWidget->setData(imageCollection->getActiveFile()->getImage().getMetadata());
    historyTableWidget->setData(imageCollection->getActiveFile()->getImage().getMetadata());
  }
}

void MainWindow::run(const QFileInfo &fileinfo)
{
  if (IOFactory::getInstance()->isScript(fileinfo.absoluteFilePath()))
  {
    if (QMessageBox::question(this,"Execute Script","Really execute the script '"+fileinfo.fileName()+"'") == QMessageBox::Yes)
    {
      QSettings settings;
      settings.setValue(AppSettings::PATH_SCRIPT,fileinfo.absolutePath());
      runScriptFile(fileinfo);
    }
  }
  else
  {
    QMessageBox::information(this,QApplication::applicationDisplayName(),"'"+fileinfo.fileName()+"' is not a known script file!");
  }
}

void MainWindow::open(const QFileInfo &fileinfo)
{
  if (IOFactory::getInstance()->isImage(fileinfo.absoluteFilePath()))
  {
    openImage(fileinfo);
  }
//  else if (IOFactory::getInstance()->isScript(fileinfo.absoluteFilePath()))
//  {
////    if (QMessageBox::question(this,"Execute Script","Really execute the script '"+fileinfo.fileName()+"'") == QMessageBox::Yes)
////    {
////      QSettings settings;
////      settings.setValue(AppSettings::PATH_SCRIPT,fileinfo.absolutePath());
////      runScriptFile(fileinfo);
////    }
//  }
  else
  {
    openExternal(fileinfo);
  }
}

void MainWindow::openExternal(const QFileInfo& file)
{
  if (!ExternalToolsLauncher().open(file))
  {
    QMessageBox::information(this,QApplication::applicationDisplayName(),"Don't know how to open file '"+file.fileName()+"'");
  }
}

void MainWindow::copyImage()
{
  if (ui->openFileList->currentIndex().isValid())
  {
    auto obj = imageCollection->getActiveFile();
    auto newobj = obj->copy("");
    imageCollection->addFile(newobj);
  }
}

void MainWindow::openImage(const QFileInfo &fileinfo)
{
  std::shared_ptr<FitsObject> loaded = imageCollection->getFile(fileinfo.absoluteFilePath());
  if (loaded)
  {
    if (QMessageBox::question(this,QApplication::applicationDisplayName(),"Image is already open!\nLoad it again?") != QMessageBox::Yes)
    {
      display(loaded);
      return;
    }
  }
  QSettings settings;
  settings.setValue(AppSettings::PATH_IMAGE,fileinfo.absolutePath());
  IOHandler* handler = IOFactory::getInstance()->getHandler(fileinfo.absoluteFilePath());
  if (handler)
  {
    try
    {
      QApplication::setOverrideCursor(Qt::BusyCursor);
      auto images = handler->read(fileinfo.absoluteFilePath());
      if (images.empty())
      {
        QMessageBox::warning(this,QApplication::applicationDisplayName(),"The file contains no images!");
      }
      ui->openFileList->selectionModel()->clearSelection();
      for (const std::shared_ptr<FitsObject>& obj : images)
      {
        imageCollection->addFile(obj);
        if (AppSettings().isLogbookLogOpen())
          logbook.add(LogbookEntry::Op,obj->getImage().getName(),"Loaded from file "+fileinfo.absoluteFilePath());
      }
      QApplication::restoreOverrideCursor();
      ui->openFileList->selectionModel()->setCurrentIndex(imageCollection->index(imageCollection->rowCount()-1,0),QItemSelectionModel::SelectCurrent);
      //      ui->scrollArea->setWidgetResizable(false);
      display(imageCollection->getFile(imageCollection->rowCount()-1));
    }
    catch (std::exception& ex)
    {
      QApplication::restoreOverrideCursor();
      QMessageBox::warning(this,QApplication::applicationDisplayName(),ex.what());
      qCritical() << ex.what();
    }
  }
  else
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"No IOHandler for:\n"+fileinfo.absoluteFilePath());
    qWarning() << "No IOHandler for " << fileinfo.absoluteFilePath();
  }
}

void MainWindow::openSelection()
{
  for (const QString& file : filesystemView->getSelectedFiles())
  {
    open(QFileInfo(file));
  }
}

void MainWindow::copySelectionToList()
{
  std::vector<QFileInfo> filelist;
  for (const QString& file : filesystemView->getSelectedFiles())
  {
    if (IOFactory::getInstance()->isImage(file))
    {
      filelist.push_back(QFileInfo(file));
    }
  }
  if (selectedFileList && !filelist.empty())
  {
    if (AppSettings().isFilelistAddOnCopy())
      selectedFileList->addFiles(filelist);
    else
      selectedFileList->setFiles(filelist);
  }
}

void MainWindow::fileListDoubleClicked(int index)
{
  if (selectedFileList && index >= 0)
  {
    QFileInfo file = selectedFileList->getFiles()[index];
    openImage(file);
  }
}

void MainWindow::fileListOpenSelected()
{
  std::vector<QFileInfo> list = filelistWidget->getSelection();
  for (const QFileInfo& file : list) openImage(file);
}

void MainWindow::openLogbook(const QString &name)
{
  AppSettings settings;
  try
  {
    std::unique_ptr<LogbookStorage> s = logbookutils::getLogboook(name);
    if (s)
    {
      logbook.open(s);
      ui->logbookDockWidget->setWindowTitle("Logbook - "+logbook.getTitle());
      logbookWidget->setLogbook(&logbook);
      settings.setLogbook(name);
    }
    else
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Unsupported logbook format!");
    }
  }
  catch (std::exception& ex)
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Failed to read logbook!");
  }
}


void MainWindow::logPluginOperation(QString image, QString op)
{
  logbook.addOp(image,op);
  if (!logbook.isActive())
  {
    qInfo() << image << op;
  }
}

void MainWindow::zoom(int z)
{
  ui->scrollArea->setWidgetResizable(z==0);
  if (z == 0)
    ui->scrollArea->layout();
  else
    imageWidget->adjustSize();
  imageWidget->setZoom(z);
}

void MainWindow::addPixel(QPoint p)
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile)
  {
    Pixel pixel = activeFile->getImage().getPixel(p.x(),p.y());
    activeFile->getPixelList()->addPixel(pixel);
  }
}

void MainWindow::setCursor(QPoint p)
{
  if (selectionMode == SelectPixel) addPixel(p);
//  ui->aoiLabel->setText("---");
}

void MainWindow::updateCursor(QPoint p)
{
  QString txt = QString::asprintf("%4d,%4d",p.x(),p.y());
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile)
  {
    txt += " : " + QString::number(activeFile->getImage().getConstPixelIterator(p.x(),p.y()).getRGB().gray());
  }
  ui->cursorLabel->setText(txt);
}

void MainWindow::updateAOI(QRect r)
{
  if (r.isNull())
  {
    ui->aoiLabel->setText("---");
  }
  else
  {
    ui->aoiLabel->setText(QString("%1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
  }
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile)
  {
    activeFile->setAOI(r);
  }
}

void MainWindow::showImageContextMenu(QPoint mouse, QPoint pixel)
{
  qInfo() << mouse << pixel;
  imageContextMenuAnchor = pixel;
  imageContextMenu->popup(imageWidget->mapToGlobal(mouse));
}

void MainWindow::runScriptCmd(const QString& cmd)
{
  try
  {
    script->runCmd(cmd);
  }
  catch (std::exception &ex)
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),ex.what());
    qCritical() << ex.what();
  }
  consoleWidget->setMode(QConsoleWidget::Input);
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile) activeFile->updateHistogram();
  updateDisplay();
}

void MainWindow::runScriptFile(const QFileInfo& fileinfo)
{
  consoleWidget->setMode(QConsoleWidget::Output);
  try
  {
    if (fileinfo.suffix() == "py")
    {
      script->runFile(fileinfo.absoluteFilePath());
    }
  }
  catch (std::exception &ex)
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),ex.what());
    qCritical() << ex.what();
  }
  consoleWidget->setMode(QConsoleWidget::Input);
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile) activeFile->updateHistogram();
  updateDisplay();
}

void MainWindow::setScriptOutput()
{
  if (scriptOutConnection) disconnect(scriptOutConnection);
  if (scriptErrConnection) disconnect(scriptErrConnection);
  if (script)
  {
    if (AppSettings().isScriptOutputToLogwidget())
    {
      scriptOutConnection = connect(script.get(),&Script::stdoutAvailable,logWidget,&LogWidget::writeStdOut);
      scriptErrConnection = connect(script.get(),&Script::stderrAvailable,logWidget,&LogWidget::writeStdErr);
    }
    else
    {
      scriptOutConnection = connect(script.get(),&Script::stdoutAvailable,consoleWidget,&ConsoleWidget::writeStdOut);
      scriptErrConnection = connect(script.get(),&Script::stderrAvailable,consoleWidget,&ConsoleWidget::writeStdErr);
    }
  }
}

std::string MainWindow::getWorkingDir() const
{
  return filesystemView->getRoot().toStdString();
}

void MainWindow::setWorkingDir(const std::string& dir)
{
  filesystemView->setRoot(QString::fromStdString(dir));
}

std::shared_ptr<FitsObject> MainWindow::get(const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(filesystemView->getRoot()+"/"+QString::fromStdString(filename));
  }
  return imageCollection->getFile(fileinfo.absoluteFilePath());
}

const std::vector<std::shared_ptr<FitsObject>>& MainWindow::getOpen() const
{
  return imageCollection->getFiles();
}

std::shared_ptr<FitsObject> MainWindow::load(const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(filesystemView->getRoot()+"/"+QString::fromStdString(filename));
  }
  IOHandler* handler = IOFactory::getInstance()->getHandler(fileinfo.absoluteFilePath());
  if (handler)
  {
    try
    {
      auto images = handler->read(fileinfo.absoluteFilePath());
      /* TODO: Handle multiple images loaded from a single file */
      return images.front();
    }
    catch (std::exception& ex)
    {
      qCritical() << ex.what();
    }
  }
  else
  {
    qWarning() << "No IOHandler for " << fileinfo.absoluteFilePath();
  }
  return std::shared_ptr<FitsObject>();
}

bool MainWindow::save(std::shared_ptr<FitsObject> obj, const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(filesystemView->getRoot()+"/"+QString::fromStdString(filename));
  }
  try
  {
    obj->save(fileinfo.absoluteFilePath());
    //    logbook.add(LogbookEntry::Op,activeFile->getImage().getName(),"Saved to file "+fn);
  }
  catch (std::exception& ex)
  {
    qCritical() << ex.what();
    return false;
  }
  return true;
}

FileList* MainWindow::getSelectedFileList() const
{
  return selectedFileList.get();
}

// void MainWindow::getStarlistFromPixellist()
// {
//   std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
//   if (activeFile)
//   {
//     StarDialog d(this);
//     if (d.exec())
//     {
//       int box = d.getBoxSize();
//       ValueType sky = 0;
//       if (d.isUserSkyValue())
//       {
//         sky = d.getUserSkyValue();
//       }
//       else
//       {
//         Histogram hist = activeFile->getHistogram();
// //        hist.build(img.get());
//         AverageResult avg = hist.getAverage(0.75);
//         sky = avg.mean;
//       }
//       StarList list(activeFile->getImage().get(),sky,activeFile->getPixelList(),box);
//       activeFile->getStarList()->setStars(list.getStars());
//       updateDisplay();
//     }
//   }
// }


void MainWindow::toggleXYChartDisplay(bool flag)
{
  if (flag)
    ui->displayWidget->setCurrentWidget(ui->chartPage);
  else
    ui->displayWidget->setCurrentWidget(ui->imagePage);
}



void MainWindow::onImageScaleChanged(double min, double max, int scale)
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile)
  {
    QImage tmp = activeFile->getImage().toQImage(min,max,static_cast<FitsImage::Scale>(scale));
    imageWidget->setImage(tmp,activeFile->getPixelList(),activeFile->getStarList());
  }
}

void MainWindow::on_actionOpen_triggered()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_IMAGE,IOFactory::getInstance()->getReadFilters());
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    openImage(info);
  }
}

void MainWindow::on_actionQuit_triggered()
{
  close();
//  QApplication::exit();
}

void MainWindow::on_actionAbout_triggered()
{
  AboutDialog().exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
  QMessageBox::aboutQt(this);
}

void MainWindow::on_actionSave_triggered()
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (!activeFile) return;
  QString fn = activeFile->getFilename();
  if (fn.isEmpty())
  {
    on_actionSave_As_triggered();
  }
  else
  {
    try
    {
      activeFile->save(fn);
      logbook.add(LogbookEntry::Op,activeFile->getImage().getName(),"Saved to file "+fn);
    }
    catch (std::exception& ex)
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),ex.what());
      qCritical() << ex.what();
    }
  }
}

void MainWindow::on_actionSave_As_triggered()
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile == nullptr) return;
  AppSettings settings;
  QString filter;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_IMAGE,IOFactory::getInstance()->getWriteFilters(),&filter);
  if (!fn.isNull())
  {
    try
    {
      fn = IOFactory::assertSuffix(fn,filter);
      activeFile->save(fn);
      logbook.add(LogbookEntry::Op,activeFile->getImage().getName(),"Saved to file "+fn);
    }
    catch (std::exception& ex)
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),ex.what());
      qCritical() << ex.what();
    }
  }
}

void MainWindow::on_actionLoad_Plugin_triggered()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_PLUGIN);
  if (!fn.isNull())
  {
    QObject* plugin = pluginFactory->loadPlugin(fn);
    if (plugin)
    {
      OpPlugin* op = dynamic_cast<OpPlugin*>(plugin);
      if (op) addOpPlugin(op);
    }
    else
    {
      qWarning() << "Failed to load plugin " << fn;
    }
  }
}

void MainWindow::on_actionUndo_triggered()
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile)
  {
    activeFile->popUndo();
    logbook.add(LogbookEntry::Op,activeFile->getImage().getName(),"Undo last operation");
    updateDisplay();
    ui->actionUndo->setEnabled(activeFile->isUndoAvailable());
  }
}

void MainWindow::on_actionMetadata_triggered()
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile)
  {
    if (!editMetadataDialog) editMetadataDialog = new EditMetadataDialog(this);
    ImageMetadata data = activeFile->getImage().getMetadata();
    editMetadataDialog->displayMetadata(data);
    if (editMetadataDialog->exec())
    {
      editMetadataDialog->commit(data);
      activeFile->getImage().setMetadata(data);
      updateMetadata();
    }
  }
}

void MainWindow::on_openFileList_clicked(const QModelIndex &index)
{
//  qDebug() << "clicked: " << index.row();
  std::shared_ptr<FitsObject> file = imageCollection->setActiveFile(index.row());
  display(file);
}

void MainWindow::on_openFileList_customContextMenuRequested(const QPoint &pos)
{
  openFileListMenu->popup(ui->openFileList->mapToGlobal(pos));
}

void MainWindow::on_actionClose_Image_triggered()
{
  if (imageCollection->getFiles().size() == 1)
  {
    on_actionClose_All_Images_triggered();
  }
  else if (ui->openFileList->currentIndex().isValid())
  {
    int row = ui->openFileList->currentIndex().row();
    /* keep old active file until all UI elements are updated with new one */
    auto old = imageCollection->removeActiveFile();
    if (static_cast<size_t>(row) >= imageCollection->getFiles().size()) row = imageCollection->getFiles().size() - 1;
    imageCollection->setActiveFile(row);
    ui->openFileList->setCurrentIndex(imageCollection->index(row,0,QModelIndex()));
    display(imageCollection->getActiveFile());
    old.reset();
  }
  else
  {
    pixellistWidget->setPixelList(defaultPixelList.get());
    starlistWidget->setStarList(defaultStarList.get());
    display(std::shared_ptr<FitsObject>());
  }
}

void MainWindow::on_actionClose_All_Images_triggered()
{
  /* first remove image from various displays! */
  display(std::shared_ptr<FitsObject>());
  pixellistWidget->setPixelList(defaultPixelList.get());
  starlistWidget->setStarList(defaultStarList.get());
  imageCollection->removeAll();
}

void MainWindow::on_actionPreferences_triggered()
{
  ConfigurationDialog *dlg = new ConfigurationDialog(this);
  if (dlg->exec())
  {
    dlg->commit();
    AppSettings settings;
    logbookWidget->rebuild();
    profileWidget->setClickEndsTracking(settings.isProfileStopTracking());
    setScriptOutput();
//    db::configure(settings);
  }
  delete dlg;
}

void MainWindow::on_actionOpen_Logbook_triggered()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_LOGBOOK,logbookutils::getFilenameFilter());
  if (!fn.isNull())
  {
    openLogbook(fn);
  }
}

void MainWindow::on_actionShow_Selected_Pixels_toggled(bool flag)
{
  AppSettings().setShowPixellist(flag);
  imageWidget->repaint();
}

void MainWindow::on_actionShow_Detected_Stars_toggled(bool flag)
{
  AppSettings().setShowStarlist(flag);
  imageWidget->repaint();
}

void MainWindow::on_actionNew_Logbook_triggered()
{
  AppSettings settings;
  QString filter;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOGBOOK,logbookutils::getFilenameFilter(),&filter);
  if (!fn.isNull())
  {
    if (!filter.startsWith("All"))
    {
      fn = IOFactory::assertSuffix(fn,filter);
    }
    openLogbook(fn);
  }
}

void MainWindow::on_actionExport_Image_triggered()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_EXPORT,"Portable Network Graphics (*.png);;JPEG (*.jpg *.jpeg);;Bitmaps (*.bmp)");
  if (!fn.isNull())
  {
    SimpleProfiler profiler("ExportImage");
    profiler.start();
    if (!imageWidget->getImage().isNull()) imageWidget->getImage().save(fn);
    profiler.stop();
  }
}

void MainWindow::on_actionNext_Image_triggered()
{
  int n = (ui->openFileList->currentIndex().row() + 1) % imageCollection->rowCount();
  ui->openFileList->setCurrentIndex(imageCollection->index(n,0,QModelIndex()));
  std::shared_ptr<FitsObject> file = imageCollection->setActiveFile(n);
  display(file);
}

void MainWindow::on_actionPrevious_Image_triggered()
{
  int n = ui->openFileList->currentIndex().row() - 1;
  if (n < 0) n += imageCollection->rowCount();
  ui->openFileList->setCurrentIndex(imageCollection->index(n,0,QModelIndex()));
  std::shared_ptr<FitsObject> file = imageCollection->setActiveFile(n);
  display(file);
}

void MainWindow::on_actionAdd_Current_Image_to_List_triggered()
{
  if (selectedFileList)
  {
    QFileInfo info(imageCollection->getActiveFile()->getFilename());
    selectedFileList->addFile(info);
  }
}

void MainWindow::on_actionExport_Logbook_triggered()
{
#ifdef HAVE_INJA
  LogbookExportDialog d(this);
  if (d.exec())
  {
    QString templ = d.getTemplate();
    QString file = d.getFilename();
    if (!(file.isEmpty() || templ.isEmpty()))
    {
      logbook.exportToFile(file,templ);
    }
  }
#else
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,"Plain Text (*.txt)");
  if (!fn.isNull())
  {
    logbook.exportToFile(fn);
  }
#endif
}

void MainWindow::on_actionClose_Logbook_triggered()
{
  logbook.close();
  ui->logbookDockWidget->setWindowTitle("Logbook - not open");
  logbookWidget->rebuild();
}

void MainWindow::on_actionProperties_triggered()
{
  LogbookPropertiesDialog d(this);
  d.setTitleAndDescription(logbook.getTitle(),logbook.getDescription());
  if (d.exec())
  {
    logbook.setTitle(d.getTitle());
    logbook.setDescription(d.getDescription());
  }
}

void MainWindow::on_actionClear_AOI_triggered()
{
  imageWidget->clearAOI();
}

void MainWindow::on_actionSelect_Pixel_toggled(bool flag)
{
  selectionMode = flag ? SelectionMode::SelectPixel : SelectionMode::None;
}

void MainWindow::on_actionRun_Script_triggered()
{
  AppSettings settings;
  QString filter = "";
#ifdef USE_PYTHON
  filter += "Python Script (*.py)";
#endif
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_SCRIPT,filter);
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    runScriptFile(info);
  }
}

void MainWindow::on_actionSave_As_PSF_triggered()
{
  std::shared_ptr<FitsObject> activeFile = imageCollection->getActiveFile();
  if (activeFile == nullptr)
  {
    QMessageBox::warning(this,"Save as PSF","No current image displayed!");
    return;
  }
  QString name = QInputDialog::getText(this,"Save as PSF","Name:");
  if (!name.isEmpty())
  {
    SimpleProfiler profiler("SavePSF");
    profiler.start();
    bool ret = PSFFactory::getInstance()->addPSF(activeFile->getImage(),name);
    profiler.stop();
    if (!ret)
    {
      QMessageBox::critical(this,"Save as PSF","Failed to save image as PSF!");
    }
  }
}


void MainWindow::on_actionPSF_Manager_triggered()
{
  if (!psfManager) psfManager = new PSFManagerDialog(imageCollection.get());
  psfManager->updatePSFList();
  psfManager->show();
}


void MainWindow::on_actionPlugins_triggered()
{
  PluginInfoDialog* d = new PluginInfoDialog(pluginFactory,this);
  d->exec();
  d->deleteLater();
}

void MainWindow::annotate(QPoint pixel)
{
  qInfo() << "annotate not implemented" << pixel;
}

void MainWindow::showSysInfo()
{
  if (!sysinfoDialog)
  {
    sysinfoDialog = new SysInfoDialog(this);
  }
  sysinfoDialog->show();
}
