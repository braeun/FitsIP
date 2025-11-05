#include "previewwidget.h"
/********************************************************************************
 *                                                                              *
 * FitsIP - widgets for previews in dialogs                                     *
 *                                                                              *
 * modified: 2024-12-16                                                         *
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

#include "ui_previewwidget.h"
#include <fitsimage.h>
#include <histogram.h>
#include <settings.h>

PreviewWidget::PreviewWidget(QWidget *parent):QWidget(parent),
  ui(new Ui::PreviewWidget)
{
  ui->setupUi(this);
}

PreviewWidget::~PreviewWidget()
{
  delete ui;
}

void PreviewWidget::clear()
{
  ui->previewLabel->setPixmap(QPixmap());
}

void PreviewWidget::setOptions(const PreviewOptions& opt)
{
  options = opt;
}

void PreviewWidget::setSourceImage(const FitsImage& image)
{
  sourceImage = image;
  updatePreview(image);
}

void PreviewWidget::setSourceImage(const FitsImage& image, QRect selection)
{
  if (selection.isEmpty())
  {
    Settings s;
    int w = s.getPreviewWidth();
    int h = s.getPreviewHeight();
    selection = QRect(image.getWidth()/2-w/2,image.getHeight()/2-h/2,w,h);
  }
  selection = image.getOverlap(selection);
  sourceImage = *image.subImage(selection);
  updatePreview(sourceImage);
}

const FitsImage& PreviewWidget::getSourceImage() const
{
  return sourceImage;
}

void PreviewWidget::updatePreview(const FitsImage& image)
{
  ui->previewLabel->setPixmap(QPixmap());
  if (image)
  {
    Histogram h;
    h.build(&image);
    QImage i = image.toQImage(h.getMin(),h.getMax(),options.scale);
    ui->previewLabel->setPixmap(QPixmap::fromImage(i));
  }
}
