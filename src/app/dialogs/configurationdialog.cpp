/********************************************************************************
 *                                                                              *
 * FitsIP - configuration dialog                                                *
 *                                                                              *
 * modified: 2022-12-03                                                         *
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
#include <QStyleFactory>
#include <QSettings>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConfigurationDialog)
{
  ui->setupUi(this);
  ui->styleBox->insertItems(0,QStyleFactory::keys());
  for (const QString& key : PaletteFactory::getPaletteNames())
  {
    ui->paletteBox->addItem(key);
  }
  AppSettings settings;
  updateFields(settings);
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
  settings.setLogbookLogOpen(ui->logLoadingBox->isChecked());
  settings.setLogbookOpenLast(ui->openLastLogBox->isChecked());

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
  ui->logLoadingBox->setChecked(settings.isLogbookLogOpen());
  ui->openLastLogBox->setChecked(settings.isLogbookOpenLast());
}
