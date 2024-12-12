/********************************************************************************
 *                                                                              *
 * FitsIP - virtual base class for dialogs with a preview                       *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

#include "abstractpreviewdialog.h"
#include <fitsimage.h>
#include <histogram.h>
#include <settings.h>
#include <QLabel>

AbstractPreviewDialog::AbstractPreviewDialog(QWidget* parent): QDialog(parent),
  previewLabel(nullptr)
{
}

AbstractPreviewDialog::~AbstractPreviewDialog()
{
}

void AbstractPreviewDialog::setPreview(std::shared_ptr<FitsImage> image, QRect selection)
{
  if (selection.isEmpty())
  {
    Settings s;
    int w = s.getPreviewWidth();
    int h = s.getPreviewHeight();
    selection = QRect(image->getWidth()/2-w/2,image->getHeight()/2-h/2,w,h);
  }
  previewImage = image->subImage(selection);
  if (previewLabel) updatePreview(previewLabel);
}


void AbstractPreviewDialog::updatePreview(QLabel *label)
{
  label->setPixmap(QPixmap());
  if (previewImage)
  {
    auto img = getPreviewImage();
    if (img)
    {
      Histogram h;
      h.build(img.get());
      QImage i = img->toQImage(h.getMin(),h.getMax(),FitsImage::LINEAR);
      label->setPixmap(QPixmap::fromImage(i));
    }
  }
}

void AbstractPreviewDialog::setPreviewLabel(QLabel *label)
{
  previewLabel = label;
}
