/********************************************************************************
 *                                                                              *
 * FitsIP - main application window                                             *
 *                                                                              *
 * modified: 2022-12-04                                                         *
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
#include "pluginfactory.h"
#include "profilertablemodel.h"
#include "appsettings.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/configurationdialog.h"
#include "dialogs/editmetadatadialog.h"
#include "dialogs/logbookpropertiesdialog.h"
#include <filelist.h>
#include <imagecollection.h>
#include <pixellist.h>
#include <io/iofactory.h>
#include <logbook/xmllogbookstorage.h>
#include <QActionGroup>
#include <QCloseEvent>
#include <QDir>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  editMetadataDialog(nullptr)
{
  ui->setupUi(this);
  ui->openFileList->setModel(&ImageCollection::getGlobal());
  imageWidget = new ImageWidget();
  imageWidget->setBackgroundRole(QPalette::Base);
  imageWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  imageWidget->setZoom(0);
  connect(imageWidget,&ImageWidget::setPixel,this,&MainWindow::addPixel);
  connect(imageWidget,&ImageWidget::cursorMoved,this,&MainWindow::updateCursor);
  connect(imageWidget,&ImageWidget::cursorMoved,ui->profileWidget,&ProfileView::updateCursor);
  connect(imageWidget,&ImageWidget::cursorSet,ui->profileWidget,&ProfileView::setCursor);
  connect(imageWidget,&ImageWidget::aoiChanged,this,&MainWindow::updateAOI);
  ui->scrollArea->setWidget(imageWidget);
  ui->scrollArea->setWidgetResizable(true);
  ui->scrollArea->setBackgroundRole(QPalette::Shadow);
  connect(ui->histogramWidget,&HistogramView::imageScaleChanged,this,&MainWindow::onImageScaleChanged);
  loadPlugins();

  connect(ui->fileSystemView,&FileSystemView::openFile,[this](QString file){open(QFileInfo(file));});
  connect(ui->fileSystemView,&FileSystemView::openSelection,this,[this]{openSelection();});
  connect(ui->fileSystemView,&FileSystemView::copySelectionToFilelist,this,[this]{copySelectionToList();});

  connect(ui->fileListWidget,&FileListWidget::openSelected,this,&MainWindow::fileListOpenSelected);

  connect(ui->consoleDockWidget,&QDockWidget::dockLocationChanged,ui->logWidget,&LogWidget::dockLocationChanged);
  connect(ui->logbookDockWidget,&QDockWidget::dockLocationChanged,ui->logbookWidget,&LogbookWidget::dockLocationChanged);

  connect(ui->actionFit_Window,&QAction::triggered,this,[=](){ zoom(0); });
  connect(ui->actionOriginal_Size,&QAction::triggered,this,[=](){ zoom(1); });
  connect(ui->action1_2,&QAction::triggered,this,[=](){ zoom(-2); });
  connect(ui->action1_4,&QAction::triggered,this,[=](){ zoom(-4); });
  connect(ui->action2_1,&QAction::triggered,this,[=](){ zoom(2); });
  connect(ui->action4_1,&QAction::triggered,this,[=](){ zoom(4); });

  ui->menuWindow->addAction(ui->fileSystemDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->fileListDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->openFilesDockWidget->toggleViewAction());
  ui->menuWindow->addSeparator();
  ui->menuWindow->addAction(ui->metadataDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->historyDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->histogramDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->profilesDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->starlistDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->pixellistDockWidget->toggleViewAction());
  ui->menuWindow->addSeparator();
  ui->menuWindow->addAction(ui->logbookDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->consoleDockWidget->toggleViewAction());
  ui->menuWindow->addAction(ui->profilerDockWidget->toggleViewAction());

  ui->fileListDockWidget->close();
  ui->profilesDockWidget->close();
  ui->starlistDockWidget->close();
  ui->pixellistDockWidget->close();
  ui->historyDockWidget->close();

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

  AppSettings settings;
  if (!settings.getLogbook().isEmpty() && settings.isLogbookOpenLast()) openLogbook(settings.getLogbook());
  ui->logbookWidget->setLogbook(&logbook);

  connect(PluginFactory::instance(),&PluginFactory::logOperation,this,&MainWindow::logPluginOperation);
  connect(ui->actionActive,&QAction::toggled,&logbook,&Logbook::activate);

  connect(ui->actionLoad_File_List,&QAction::triggered,ui->fileListWidget,&FileListWidget::load);
  connect(ui->actionSave_File_List,&QAction::triggered,ui->fileListWidget,&FileListWidget::save);

  connect(PluginFactory::instance(),&PluginFactory::logProfilerResult,ui->profilerWidget->getModel(),&ProfilerTableModel::addProfilerResult);
  connect(IOFactory::getInstance(),&IOFactory::logProfilerResult,ui->profilerWidget->getModel(),&ProfilerTableModel::addProfilerResult);

  QActionGroup* grp = new QActionGroup(this);
  ui->actionTable->setActionGroup(grp);
  ui->actionBy_Date->setActionGroup(grp);
  ui->actionTree_By_Project->setActionGroup(grp);
  ui->actionTree_By_Project_and_Date->setActionGroup(grp);
  ui->actionTree_By_Project_and_Step->setActionGroup(grp);
  connect(ui->actionTable,&QAction::triggered,[=](bool checked){if (checked) ui->logbookWidget->setDisplay(LogbookWidget::Table);});
  connect(ui->actionBy_Date,&QAction::triggered,[=](bool checked){if (checked) ui->logbookWidget->setDisplay(LogbookWidget::TreeByDate);});
  connect(ui->actionTree_By_Project,&QAction::triggered,[=](bool checked){if (checked) ui->logbookWidget->setDisplay(LogbookWidget::TreeByProject);});
  connect(ui->actionTree_By_Project_and_Date,&QAction::triggered,[=](bool checked){if (checked) ui->logbookWidget->setDisplay(LogbookWidget::TreeByProjectByDate);});
  connect(ui->actionTree_By_Project_and_Step,&QAction::triggered,[=](bool checked){if (checked) ui->logbookWidget->setDisplay(LogbookWidget::TreeByProjectByStep);});
  switch (settings.getLogwidgetStyle())
  {
    case 0:
      ui->actionTable->setChecked(true);
      ui->logbookWidget->setDisplay(LogbookWidget::Table);
      break;
    case 1:
      ui->actionBy_Date->setChecked(true);
      ui->logbookWidget->setDisplay(LogbookWidget::TreeByDate);
      break;
    case 2:
      ui->actionTree_By_Project->setChecked(true);
      ui->logbookWidget->setDisplay(LogbookWidget::TreeByProject);
      break;
    case 3:
      ui->actionTree_By_Project_and_Date->setChecked(true);
      ui->logbookWidget->setDisplay(LogbookWidget::TreeByProjectByDate);
      break;
    case 4:
      ui->actionTree_By_Project_and_Step->setChecked(true);
      ui->logbookWidget->setDisplay(LogbookWidget::TreeByProjectByStep);
      break;
  }
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue(AppSettings::MAINWINDOW_STATE,saveState());
  settings.setValue(AppSettings::MAINWINDOW_GEOMETRY,saveGeometry());
  delete ui;
}

void MainWindow::initialize()
{
}

LogWidget* MainWindow::getLogWidget()
{
  return ui->logWidget;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
}

void MainWindow::loadPlugins()
{
  QObjectList list = PluginFactory::instance()->getPlugins();
  for (QObject* o : list)
  {
    OpPlugin* op = dynamic_cast<OpPlugin*>(o);
    if (op != nullptr) addOpPlugin(op);
  }
}

void MainWindow::addOpPlugin(OpPlugin *op)
{
  if (op->getMenuEntry().isEmpty())
  {
    qWarning() << "No menu entry provided for plugin!";
    return;
  }
  QAction* a = addMenuEntry(op->getMenuEntry());
  if (a != nullptr)
  {
    PluginMenuEntry entry;
    entry.action = a;
    entry.plugin = op;
    pluginMenus.push_back(entry);
    connect(a,&QAction::triggered,this,[=](){this->executeOpPlugin(op);});
  }
}

QAction* MainWindow::addMenuEntry(QString entry)
{
  QStringList entries = entry.split("/");
  if (entries.size() < 2)
  {
    qWarning() << "Illegal menu entry provided for plugin";
    return nullptr;
  }
  QMenu* menu = nullptr;
  QMenu* lastmenu = nullptr;
  QList<QAction*> actions = ui->menubar->actions();
  for (int32_t i=0;i<entries.size()-1;i++)
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
      if (i == 0)
      {
        for (QAction* a : actions)
        {
          if (a->text() == "Window")
          {
            ui->menubar->insertMenu(a,menu);
            break;
          }
        }
      }
      else
      {
        lastmenu->addMenu(menu);
      }
    }
    actions = menu->actions();
    lastmenu = menu;
    menu = nullptr;
  }
  if (!lastmenu) return nullptr;
  QAction* action = lastmenu->addAction(entries.last());
  return action;
}

void MainWindow::executeOpPlugin(OpPlugin *op)
{
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (op->requiresFileList())
  {
    std::vector<QFileInfo> filelist = getFileList();
    if (!filelist.empty())
    {
      OpPlugin::ResultType ret = op->execute(filelist,imageWidget->getAOI());
      if (ret == OpPlugin::OK)
      {
        if (!op->getFileList().empty()) ui->fileListWidget->getFileList()->setFiles(op->getFileList());
        if (op->createsNewImage())
        {
          for (auto img : op->getCreatedImages())
          {
            std::shared_ptr<FileObject> file = std::make_shared<FileObject>("",img);
            ImageCollection::getGlobal().addFile(file);
          }
          ui->openFileList->selectionModel()->clearSelection();
          ui->openFileList->selectionModel()->setCurrentIndex(ImageCollection::getGlobal().index(ImageCollection::getGlobal().rowCount()-1,0),QItemSelectionModel::SelectCurrent);
    //      ui->scrollArea->setWidgetResizable(false);
          ImageCollection::getGlobal().setActiveFile(ImageCollection::getGlobal().rowCount()-1);
          display(ImageCollection::getGlobal().getActiveFile());
        }
      }
      else if (ret == OpPlugin::ERROR)
      {
        QMessageBox::warning(this,QApplication::applicationDisplayName(),"Plugin execution error!\n"+op->getError());
      }
    }
    else
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Plugin requires a list of files!");
    }
  }
  else if (activeFile != nullptr)
  {
    qDebug() << "Executing: " << op->getMenuEntry();
    activeFile->pushUndo();
    ui->actionUndo->setEnabled(activeFile->isUndoAvailable());
    executeOpPlugin(op,activeFile->getImage(),imageWidget->getAOI());
  }
  else if (!op->requiresImage())
  {
    executeOpPlugin(op,std::shared_ptr<FitsImage>(),QRect());
  }
  else
  {
    QMessageBox::information(this,QApplication::applicationDisplayName(),"No image displayed!");
  }
}

std::vector<QFileInfo> MainWindow::getFileList()
{
  int32_t src = -1;
  QStringList files = ui->fileSystemView->getSelectedFiles();
  if (!ui->fileListWidget->getFileList()->getFiles().empty())
  {
    src = 0;
  }
  else if (!files.isEmpty())
  {
    src = 1;
  }
  else
  {
    src = 2;
  }
  std::vector<QFileInfo> filelist;
  switch (src)
  {
    case 0:
      filelist = ui->fileListWidget->getFileList()->getFiles();
      break;
    case 1:
      for (const QString& file : files)
      {
        filelist.push_back(QFileInfo(file));
      }
      break;
    case 2:
      for (const std::shared_ptr<FileObject>& file : ImageCollection::getGlobal().getFiles())
      {
        filelist.push_back(QFileInfo(file->getFilename()));
      }
      break;
  }
  return filelist;
}

void MainWindow::executeOpPlugin(OpPlugin *op, std::shared_ptr<FitsImage> img, QRect aoi)
{
  OpPlugin::ResultType ret = op->execute(img,aoi);
  if (ret == OpPlugin::OK)
  {
    if (op->createsNewImage())
    {
      for (auto img : op->getCreatedImages())
      {
        std::shared_ptr<FileObject> file = std::make_shared<FileObject>("",img);
        ImageCollection::getGlobal().addFile(file);
      }
      ui->openFileList->selectionModel()->clearSelection();
      ui->openFileList->selectionModel()->setCurrentIndex(ImageCollection::getGlobal().index(ImageCollection::getGlobal().rowCount()-1,0),QItemSelectionModel::SelectCurrent);
//      ui->scrollArea->setWidgetResizable(false);
      ImageCollection::getGlobal().setActiveFile(ImageCollection::getGlobal().rowCount()-1);
      display(ImageCollection::getGlobal().getActiveFile());
    }
    else
    {
      std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
      if (activeFile) activeFile->updateHistogram();
      updateDisplay();
    }
    if (!op->getFileList().empty()) ui->fileListWidget->getFileList()->setFiles(op->getFileList());
  }
  else if (ret == OpPlugin::ERROR)
  {
    QMessageBox::warning(this,QApplication::applicationDisplayName(),"Plugin execution error!\n"+op->getError());
  }
}

void MainWindow::display(std::shared_ptr<FileObject> file)
{
  ImageCollection::getGlobal().setActiveFile(file);
  ui->historyTable->clearContents();
  ui->historyTable->setRowCount(0);
  if (file)
  {
    QFileInfo info(file->getFilename());
    ui->activeFileLabel->setText(info.fileName());
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
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  ui->histogramWidget->setImage(activeFile);
  ui->profileWidget->setImage(activeFile);
  if (activeFile)
  {
    QImage tmp = activeFile->getImage()->toQImage(activeFile->getHistogram()->getMin(),activeFile->getHistogram()->getMax(),static_cast<FitsImage::Scale>(ui->histogramWidget->getImageScale()));
    imageWidget->setImage(tmp);
    if (!ui->scrollArea->widgetResizable()) imageWidget->adjustSize();
    ui->widthLabel->setText(QString::number(activeFile->getImage()->getWidth()));
    ui->heightLabel->setText(QString::number(activeFile->getImage()->getHeight()));
    switch (activeFile->getImage()->getDepth())
    {
      case 1:
        ui->colorLabel->setText("GRAY");
        break;
      case 2:
        ui->colorLabel->setText("FFT");
        break;
      case 3:
        ui->colorLabel->setText("RGB");
        break;
      default:
        ui->colorLabel->setText("---");
        break;
    }
  }
  else
  {
    imageWidget->setImage(QImage());
  }
  updateMetadata();
  profiler.stop();
  if (activeFile)
  {
    ui->profilerWidget->getModel()->addProfilerResult(QString::fromStdString(profiler.getName()),
                                                      activeFile->getImage()->getName(),
                                                      activeFile->getImage()->getWidth(),
                                                      activeFile->getImage()->getHeight(),
                                                      profiler.getDuration(),
                                                      "");
  }
}

void MainWindow::updateMetadata()
{
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (activeFile)
  {
    const ImageMetadata& metadata = activeFile->getImage()->getMetadata();
    ui->metadataTable->item(0,0)->setText(metadata.object);
    ui->metadataTable->item(1,0)->setText(metadata.date.toString(Qt::ISODate));
    ui->metadataTable->item(2,0)->setText(metadata.observer);
    ui->metadataTable->item(3,0)->setText(metadata.telescope);
    ui->metadataTable->item(4,0)->setText(metadata.instrument);
    ui->metadataTable->item(5,0)->setText(QString::number(metadata.exposure));
    int32_t first = ui->historyTable->rowCount();
    ui->historyTable->setRowCount(metadata.history.size());
    for (int32_t i=first;i<metadata.history.size();i++)
    {
      ui->historyTable->setItem(i,0,new QTableWidgetItem(metadata.history[i]));
    }
  }
  else
  {
    ui->historyTable->clearContents();
    ui->historyTable->setRowCount(0);
  }
}

void MainWindow::open(const QFileInfo &fileinfo)
{
  std::shared_ptr<FileObject> loaded = ImageCollection::getGlobal().getFile(fileinfo.absoluteFilePath());
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
      std::shared_ptr<FitsImage> image = handler->read(fileinfo.absoluteFilePath());
      std::shared_ptr<FileObject> file = std::make_shared<FileObject>(fileinfo.absoluteFilePath(),image);
      ImageCollection::getGlobal().addFile(file);
      ui->openFileList->selectionModel()->clearSelection();
      ui->openFileList->selectionModel()->setCurrentIndex(ImageCollection::getGlobal().index(ImageCollection::getGlobal().rowCount()-1,0),QItemSelectionModel::SelectCurrent);
//      ui->scrollArea->setWidgetResizable(false);
      display(file);
      if (AppSettings().isLogbookLogOpen())
        logbook.add(LogbookEntry::Op,image->getName(),"Loaded from file "+fileinfo.absoluteFilePath());
      QApplication::restoreOverrideCursor();
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
  for (const QString& file : ui->fileSystemView->getSelectedFiles())
  {
    open(QFileInfo(file));
  }
}

void MainWindow::copySelectionToList()
{
  std::vector<QFileInfo> filelist;
  for (const QString& file : ui->fileSystemView->getSelectedFiles())
  {
    filelist.push_back(QFileInfo(file));
  }
  if (!filelist.empty())
  {
    if (AppSettings().isFilelistAddOnCopy())
      ui->fileListWidget->getFileList()->addFiles(filelist);
    else
      ui->fileListWidget->getFileList()->setFiles(filelist);
  }
}

void MainWindow::fileListDoubleClicked(int index)
{
  QFileInfo file = ui->fileListWidget->getFileList()->getFiles()[index];
  open(file);
}

void MainWindow::fileListOpenSelected()
{
  std::vector<QFileInfo> list = ui->fileListWidget->getSelection();
  for (const QFileInfo& file : list) open(file);
}

void MainWindow::openLogbook(const QString &name)
{
  AppSettings settings;
  try
  {
    std::unique_ptr<LogbookStorage> s = std::make_unique<XMLLogbookStorage>(name);
    logbook.open(s);
    ui->logbookDockWidget->setWindowTitle("Logbook - "+logbook.getTitle());
    ui->logbookWidget->setLogbook(&logbook);
    settings.setLogbook(name);
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

void MainWindow::zoom(int32_t z)
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
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (activeFile)
  {
    Pixel pixel = activeFile->getImage()->getPixel(p.x(),p.y());
    PixelList::getGlobalInstance()->addPixel(pixel);
  }
}

void MainWindow::updateCursor(QPoint p)
{
  ui->xLabel->setText(QString::number(p.x()));
  ui->yLabel->setText(QString::number(p.y()));
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (activeFile)
  {
    ui->valueLabel->setText(QString::number(activeFile->getImage()->getConstPixelIterator(p.x(),p.y()).getRGB().gray()));
  }
}

void MainWindow::updateAOI(QRect r)
{
  ui->aoiLabel->setText(QString("%1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()));
}

void MainWindow::onImageScaleChanged(double min, double max, int32_t scale)
{
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (activeFile)
  {
    QImage tmp = activeFile->getImage()->toQImage(min,max,static_cast<FitsImage::Scale>(scale));
    imageWidget->setImage(tmp);
  }
}

void MainWindow::on_actionOpen_triggered()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_IMAGE,IOFactory::getInstance()->getReadFilters());
  if (!fn.isNull())
  {
    QFileInfo info(fn);
    open(info);
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
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
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
      logbook.add(LogbookEntry::Op,activeFile->getImage()->getName(),"Saved to file "+fn);
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
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
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
      logbook.add(LogbookEntry::Op,activeFile->getImage()->getName(),"Saved to file "+fn);
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
    Plugin* plugin = PluginFactory::instance()->loadPlugin(fn);
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
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (activeFile)
  {
    activeFile->popUndo();
    logbook.add(LogbookEntry::Op,activeFile->getImage()->getName(),"Undo last operation");
    updateDisplay();
    ui->actionUndo->setEnabled(activeFile->isUndoAvailable());
  }
}

void MainWindow::on_actionMetadata_triggered()
{
  std::shared_ptr<FileObject> activeFile = ImageCollection::getGlobal().getActiveFile();
  if (activeFile)
  {
    if (!editMetadataDialog) editMetadataDialog = new EditMetadataDialog(this);
    ImageMetadata data = activeFile->getImage()->getMetadata();
    editMetadataDialog->displayMetadata(data);
    if (editMetadataDialog->exec())
    {
      editMetadataDialog->commit(data);
      activeFile->getImage()->setMetadata(data);
      updateMetadata();
    }
  }
}

void MainWindow::on_openFileList_clicked(const QModelIndex &index)
{
//  qDebug() << "clicked: " << index.row();
  std::shared_ptr<FileObject> file = ImageCollection::getGlobal().setActiveFile(index.row());
  display(file);
}

void MainWindow::on_actionClose_Image_triggered()
{
  if (ImageCollection::getGlobal().getFiles().size() == 1)
  {
    on_actionClose_All_Images_triggered();
  }
  else
  {
    int32_t row = ui->openFileList->currentIndex().row();
    ImageCollection::getGlobal().removeActiveFile();
    if (static_cast<uint32_t>(row) >= ImageCollection::getGlobal().getFiles().size()) row = ImageCollection::getGlobal().getFiles().size() - 1;
    ImageCollection::getGlobal().setActiveFile(row);
    ui->openFileList->setCurrentIndex(ImageCollection::getGlobal().index(row,0,QModelIndex()));
    display(ImageCollection::getGlobal().getActiveFile());
  }
}

void MainWindow::on_actionClose_All_Images_triggered()
{
  ImageCollection::getGlobal().removeAll();
  display(std::shared_ptr<FileObject>());
}

void MainWindow::on_actionPreferences_triggered()
{
  ConfigurationDialog *dlg = new ConfigurationDialog(this);
  if (dlg->exec())
  {
    dlg->commit();
    ui->logbookWidget->rebuild();
  }
  delete dlg;
}

void MainWindow::on_actionOpen_Logbook_triggered()
{
  AppSettings settings;
  QString fn = settings.getOpenFilename(this,AppSettings::PATH_LOGBOOK,"File list (*.lbk);;All files (*)");
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
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOGBOOK,"File list (*.lbk);;All files (*)");
  if (!fn.isNull())
  {
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
  int n = (ui->openFileList->currentIndex().row() + 1) % ImageCollection::getGlobal().rowCount();
  ui->openFileList->setCurrentIndex(ImageCollection::getGlobal().index(n,0,QModelIndex()));
  std::shared_ptr<FileObject> file = ImageCollection::getGlobal().setActiveFile(n);
  display(file);
}

void MainWindow::on_actionPrevious_Image_triggered()
{
  int n = ui->openFileList->currentIndex().row() - 1;
  if (n < 0) n += ImageCollection::getGlobal().rowCount();
  ui->openFileList->setCurrentIndex(ImageCollection::getGlobal().index(n,0,QModelIndex()));
  std::shared_ptr<FileObject> file = ImageCollection::getGlobal().setActiveFile(n);
  display(file);
}

void MainWindow::on_actionAdd_Current_Image_to_List_triggered()
{
  QFileInfo info(ImageCollection::getGlobal().getActiveFile()->getFilename());
  ui->fileListWidget->getFileList()->addFile(info);
}

void MainWindow::on_actionExport_Logbook_triggered()
{
  AppSettings settings;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,"Plain Text (*.txt);;HTML (*.html)");
  if (!fn.isNull())
  {
    logbook.exportToFile(fn);
  }
}

void MainWindow::on_actionClose_Logbook_triggered()
{
  logbook.close();
  ui->logbookDockWidget->setWindowTitle("Logbook - not open");
  ui->logbookWidget->rebuild();
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

