/********************************************************************************
 *                                                                              *
 * FitsIP - configuration dialog                                                *
 *                                                                              *
 * modified: 2025-03-13                                                         *
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

#include "configurationdialog.h"
#include "ui_configurationdialog.h"
#include "../appsettings.h"
#include "../palettefactory.h"
#include <fitsip/core/db/database.h>
#include <QFileDialog>
#include <QStyleFactory>
#include <QSettings>

const char* bindir = "/usr/bin";

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConfigurationDialog)
{
  ui->setupUi(this);
  ui->dbDriverBox->addItems(Database::getDrivers());
  ui->styleBox->insertItems(0,QStyleFactory::keys());
  for (const QString& key : PaletteFactory::getPaletteNames())
  {
    ui->paletteBox->addItem(key);
  }
  AppSettings settings;
  updateFields(settings);
  connect(ui->browseFileManagerButton,&QPushButton::clicked,this,[this](){browse(ui->fileManagerField,bindir);});
  connect(ui->browseScriptEditorButton,&QPushButton::clicked,this,[this](){browse(ui->scriptEditorField,bindir);});
  connect(ui->browseTextEditorButton,&QPushButton::clicked,this,[this](){browse(ui->textEditorField,bindir);});
  connect(ui->browseOfficeButton,&QPushButton::clicked,this,[this](){browse(ui->officeTextEditorField,bindir);});
  connect(ui->browseInternalButton,&QPushButton::clicked,this,[this](){browse(ui->internalDirectoryField,AppSettings().getInternalDirectory());});
}

ConfigurationDialog::~ConfigurationDialog()
{
  delete ui;
}

void ConfigurationDialog::commit()
{
  AppSettings settings;
  commitFields(settings);
}

void ConfigurationDialog::commitFields(AppSettings &settings)
{
  settings.setStyle(ui->styleBox->currentText());
  settings.setPalette(ui->paletteBox->currentText());
  settings.setAlwaysSaveFits(ui->alwaysSaveFitsBox->isChecked());
  settings.setWriteMetadataFile(ui->saveMetadataBox->isChecked());
  if (ui->fitsDoubleButton->isChecked())

    settings.setFitsImageFormat(0);
  else
    settings.setFitsImageFormat(1);
  settings.setInternalDirectory(ui->internalDirectoryField->text());
  settings.setLogbookLogOpen(ui->logLoadingBox->isChecked());
  settings.setLogbookOpenLast(ui->openLastLogBox->isChecked());
  settings.setLogbookLatestFirst(ui->showLatestFirstBox->isChecked());
  settings.setProfileStopTracking(ui->profileClickBox->isChecked());
  settings.setScriptOutputToLogwidget(ui->logWidgetOutputBox->isChecked());
  settings.setTool(Settings::FileManager,ui->fileManagerField->text());
  settings.setTool(Settings::ScriptEditor,ui->scriptEditorField->text());
  settings.setTool(Settings::TextEditor,ui->textEditorField->text());
  settings.setTool(Settings::OfficeEditor,ui->officeTextEditorField->text());
  settings.setDatabaseDriver(ui->dbDriverBox->currentText());
  settings.setDatabase(ui->dbNameField->text());
  settings.setDatabaseHost(ui->dbHostField->text());
  settings.setDatabaseUser(ui->dbUserField->text());
  settings.setDatabasePassword(ui->dbPasswordField->text());

  QString style = settings.getStyle();
  QApplication::setStyle(QStyleFactory::create(style));
  QString palette = settings.getPalette();
  QApplication::setPalette(PaletteFactory::getPalette(palette));
}

void ConfigurationDialog::updateFields(const AppSettings &settings)
{
  ui->styleBox->setCurrentText(settings.getStyle());
  ui->paletteBox->setCurrentText(settings.getPalette());
  ui->alwaysSaveFitsBox->setChecked(settings.isAlwaysSaveFits());
  ui->saveMetadataBox->setChecked(settings.isWriteMetadataFile());
  switch (settings.getFitsImageFormat())
  {
    case 0:
      ui->fitsDoubleButton->setChecked(true);
      ui->fitsFloatButton->setChecked(false);
      break;
    case 1:
      ui->fitsDoubleButton->setChecked(false);
      ui->fitsFloatButton->setChecked(true);
      break;
  }
  ui->internalDirectoryField->setText(settings.getInternalDirectory());
  ui->logLoadingBox->setChecked(settings.isLogbookLogOpen());
  ui->openLastLogBox->setChecked(settings.isLogbookOpenLast());
  ui->showLatestFirstBox->setChecked(settings.isLogbookLatestFirst());
  ui->profileClickBox->setChecked(settings.isProfileStopTracking());
  ui->logWidgetOutputBox->setChecked(settings.isScriptOutputToLogwidget());
  ui->fileManagerField->setText(settings.getTool(Settings::FileManager));
  ui->scriptEditorField->setText(settings.getTool(Settings::ScriptEditor));
  ui->officeTextEditorField->setText(settings.getTool(Settings::OfficeEditor));
  ui->textEditorField->setText(settings.getTool(Settings::TextEditor));
  if (Database::getDrivers().contains(settings.getDatabaseDriver()))
  {
    ui->dbDriverBox->setCurrentText(settings.getDatabaseDriver());
  }
  else
  {
    ui->dbDriverBox->setCurrentIndex(0);
  }
  ui->dbNameField->setText(settings.getDatabase());
  ui->dbHostField->setText(settings.getDatabaseHost());
  ui->dbUserField->setText(settings.getDatabaseUser());
  ui->dbPasswordField->setText(settings.getDatabasePassword());
}

void ConfigurationDialog::browse(QLineEdit* field, QString path)
{
  QString filename = QFileDialog::getOpenFileName(this,QApplication::applicationDisplayName(),path);
  if (!filename.isNull())
  {
    field->setText(filename);
  }
}

