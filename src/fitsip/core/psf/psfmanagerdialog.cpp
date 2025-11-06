/********************************************************************************
 *                                                                              *
 * FitsIP - PSF manager dialog                                                  *
 *                                                                              *
 * modified: 2025-06-06                                                         *
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

#include "psfmanagerdialog.h"
#include "ui_psfmanagerdialog.h"
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/psf/psffactory.h>
#include <fitsip/core/dialogs/imageselectdialog.h>
#include <fitsip/core/widgets/previewwidget.h>
#include <fitsip/core/widgets/psfwidget.h>
#include <QInputDialog>
#include <QMessageBox>

PSFManagerDialog::PSFManagerDialog(ImageCollection* collection, QWidget *parent):QDialog(parent),
  ui(new Ui::PSFManagerDialog),
  collection(collection)
{
  ui->setupUi(this);
  psfWidget = new PSFWidget();
  ui->psfControlLayout->insertWidget(0,psfWidget,1);
  previewWidget = new PreviewWidget(ui->groupBox_2);
  previewWidget->setObjectName(QString::fromUtf8("previewWidget"));
  QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy3.setHorizontalStretch(1);
  sizePolicy3.setVerticalStretch(1);
  sizePolicy3.setHeightForWidth(previewWidget->sizePolicy().hasHeightForWidth());
  previewWidget->setSizePolicy(sizePolicy3);
  ui->gridLayout->addWidget(previewWidget, 0, 0, 1, 6);
  ui->gridLayout->removeWidget(ui->dummyWidget);

  connect(ui->refreshButton,&QPushButton::clicked,this,[this](){updatePSFList();});
  connect(ui->previewButton,&QPushButton::clicked,this,[this](){preview();});
  connect(ui->addImageButton,&QPushButton::clicked,this,[this](){addImagePSF();});
  connect(ui->deleteButton,&QPushButton::clicked,this,[this](){deletePSF();});
  connect(ui->renameButton,&QPushButton::clicked,this,[this](){renamePSF();});
  connect(ui->fftInputBox,&QCheckBox::stateChanged,this,[this](){preview();});
  connect(ui->previewWidthBox,qOverload<int>(&QSpinBox::valueChanged),this,[this](int){preview();});
  connect(ui->previewHeightBox,qOverload<int>(&QSpinBox::valueChanged),this,[this](int){preview();});
  connect(psfWidget,&PSFWidget::functionSelected,this,[this](QString name){functionSelected(name);});
}

PSFManagerDialog::~PSFManagerDialog()
{
  delete ui;
}

void PSFManagerDialog::updatePSFList()
{
  psfWidget->updatePSFList();
}

void PSFManagerDialog::functionSelected(QString name)
{
  const PSF* psf = PSFFactory::getInstance()->getPSF(name);
  ui->previewWidthBox->setEnabled(!psf->isFixedSize());
  ui->previewHeightBox->setEnabled(!psf->isFixedSize());
  ui->renameButton->setEnabled(PSFFactory::getInstance()->canRename(name));
  ui->deleteButton->setEnabled(PSFFactory::getInstance()->canDelete(name));
}

void PSFManagerDialog::preview()
{
  int w = ui->previewWidthBox->value();
  int h = ui->previewHeightBox->value();
  const PSF* psf = PSFFactory::getInstance()->getPSF(psfWidget->getFunction());
  auto psfpar = psfWidget->getParameters();
  if (psf)
  {
    FitsImage img;
    if (ui->fftInputBox->isChecked())
      img = psf->createPSF(w,h,psfpar);
    else
      img = psf->createPSFForDisplay(w,h,psfpar);
    previewWidget->setSourceImage(img);
  }
}

void PSFManagerDialog::addImagePSF()
{
  ImageSelectDialog d(this);
  d.setImageCollection(collection);
  if (d.exec())
  {
    std::shared_ptr<FitsObject> img = d.getImage();
    if (img)
    {
      QString name = QInputDialog::getText(this,"Save as PSF","Name:");
      if (!name.isEmpty())
      {
        FitsImage fitsimg = img->getImage();
        QRect aoi = img->getAOI();
        if (aoi.isValid())
        {
          if (QMessageBox::question(this,"Save as PSF","Crop to defined AOI?") == QMessageBox::Yes)
          {
            fitsimg = fitsimg.subImage(aoi);
          }
        }
        bool ret = PSFFactory::getInstance()->addPSF(fitsimg,name);
        if (!ret)
        {
          QMessageBox::critical(this,"Save as PSF","Failed to save image as PSF!");
        }
      }
    }
    updatePSFList();
  }
}

void PSFManagerDialog::deletePSF()
{
  QString name = psfWidget->getFunction();
  if (QMessageBox::question(this,QApplication::applicationDisplayName(),
                            "Really delete the PSF '"+name+"'?") == QMessageBox::Yes)
  {
    bool ret = PSFFactory::getInstance()->deletePSF(name);
    if (!ret)
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Failed to delete the PSF!");
    }
    else
    {
      psfWidget->updatePSFList();
    }
  }
}

void PSFManagerDialog::renamePSF()
{
  QString name = psfWidget->getFunction();
  QString newname = QInputDialog::getText(this,"Rename PSF","New name:",QLineEdit::Normal,name);
  if (!newname.isEmpty())
  {
    bool ret = PSFFactory::getInstance()->renamePSF(name,newname);
    if (!ret)
    {
      QMessageBox::warning(this,QApplication::applicationDisplayName(),"Failed to rename the PSF!");
    }
    else
    {
      psfWidget->updatePSFList();
    }
  }
}
