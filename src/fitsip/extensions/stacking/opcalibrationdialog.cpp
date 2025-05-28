/********************************************************************************
 *                                                                              *
 * FitsIP - dialog for image calibration with flatfield and dark image          *
 *                                                                              *
 * modified: 2024-12-13                                                         *
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

#include "opcalibrationdialog.h"
#include "ui_opcalibrationdialog.h"
#include <fitsip/core/settings.h>
#include <fitsip/core/dialogs/imageselectwidget.h>
#include <QSettings>
#include <QFileDialog>

OpCalibrationDialog::OpCalibrationDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpCalibrationDialog)
{
  ui->setupUi(this);
  darkFrameSelectWidget = new ImageSelectWidget(ui->groupBox);
  darkFrameSelectWidget->setObjectName(QString::fromUtf8("darkFrameSelectWidget"));
  QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(darkFrameSelectWidget->sizePolicy().hasHeightForWidth());
  darkFrameSelectWidget->setSizePolicy(sizePolicy1);
  ui->verticalLayout_2->addWidget(darkFrameSelectWidget);
  flatFieldSelectWidget = new ImageSelectWidget(ui->groupBox_2);
  flatFieldSelectWidget->setObjectName(QString::fromUtf8("flatFieldSelectWidget"));
  ui->verticalLayout_3->addWidget(flatFieldSelectWidget);

  darkFrameSelectWidget->setNoneAllowed(true);
  flatFieldSelectWidget->setNoneAllowed(true);
  connect(ui->browseButton,&QAbstractButton::clicked,this,[this]{browse();});
}

OpCalibrationDialog::~OpCalibrationDialog()
{
  delete ui;
}

void OpCalibrationDialog::setImageCollection(ImageCollection* c)
{
  darkFrameSelectWidget->setImageCollection(c);
  flatFieldSelectWidget->setImageCollection(c);
}

std::shared_ptr<FitsObject> OpCalibrationDialog::getDarkFrame()
{
  return darkFrameSelectWidget->getImage();
}

std::shared_ptr<FitsObject> OpCalibrationDialog::getFlatField()
{
  return flatFieldSelectWidget->getImage();
}

void OpCalibrationDialog::setOutputPath(const QString &path)
{
  ui->outputFolderField->setText(path);
}

QString OpCalibrationDialog::getOutputPath() const
{
  return ui->outputFolderField->text();
}

QString OpCalibrationDialog::getPrefix() const
{
  return ui->outputPrefixField->text();
}

QString OpCalibrationDialog::getSuffix() const
{
  return ui->outputSuffixField->text();
}

void OpCalibrationDialog::browse()
{
  QSettings settings;
  QString path = settings.value(Settings::PATH_IMAGE,".").toString();
  QString fn = QFileDialog::getExistingDirectory(this,QApplication::applicationDisplayName(),path);
  if (!fn.isNull())
  {
    ui->outputFolderField->setText(fn);
  }
}

