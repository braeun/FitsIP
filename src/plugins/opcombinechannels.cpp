/********************************************************************************
 *                                                                              *
 * FitsIP - combine RGB channels                                                *
 *                                                                              *
 * modified: 2024-12-12                                                         *
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

#include "opcombinechannels.h"
#include "opcombinechannelsdialog.h"
#include <fitsbase/imagecollection.h>
#include <fitsbase/fitsobject.h>

OpCombineChannels::OpCombineChannels():
  dlg(nullptr)
{
}

OpCombineChannels::~OpCombineChannels()
{
  if (dlg) delete dlg;
}

bool OpCombineChannels::requiresImage() const
{
  return false;
}

bool OpCombineChannels::createsNewImage() const
{
  return true;
}

std::vector<std::shared_ptr<FitsImage>> OpCombineChannels::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsImage>>{img};
}

QString OpCombineChannels::getMenuEntry() const
{
  return "Color/Combine...";
}

OpPlugin::ResultType OpCombineChannels::execute(std::shared_ptr<FitsImage> /*image*/, QRect selection, const PreviewOptions& opt)
{
  ImageCollection collection;
  for (const auto& img : ImageCollection::getGlobal().getFiles())
  {
    if (img->getImage()->getDepth() == 1)
    {
      collection.addFile(img);
    }
  }
  if (collection.size() < 3)
  {
    const char* msg = "At least 3 images with depth 1 must be loaded";
    setError(msg);
    qWarning() << msg;
    return ERROR;
  }
  if (!dlg) dlg = new OpCombineChannelsDialog();
  dlg->setCollection(collection);
  if (dlg->exec())
  {
    auto r = collection.getFile(dlg->getRedImage())->getImage();
    auto g = collection.getFile(dlg->getGreenImage())->getImage();
    auto b = collection.getFile(dlg->getBlueImage())->getImage();
    if (r->isCompatible(*g.get()) && r->isCompatible(*b.get()))
    {
      img = std::make_shared<FitsImage>(r->getName()+"_RGB",r->getWidth(),r->getHeight(),3);
      img->setMetadata(r->getMetadata());
      PixelIterator dest = img->getPixelIterator();
      ConstPixelIterator ri = r->getConstPixelIterator();
      ConstPixelIterator gi = g->getConstPixelIterator();
      ConstPixelIterator bi = b->getConstPixelIterator();
      while (ri.hasNext())
      {
        dest[0] = ri[0];
        dest[1] = gi[0];
        dest[2] = bi[0];
        ++dest;
        ++ri;
        ++gi;
        ++bi;
      }
    }
    else
    {
      const char* msg = "Images are not compatible";
      setError(msg);
      qWarning() << msg;
      return ERROR;
    }
  }
  return OK;
}
