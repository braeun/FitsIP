/********************************************************************************
 *                                                                              *
 * FitsIP - combine RGB channels                                                *
 *                                                                              *
 * modified: 2025-03-08                                                         *
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
#include <fitsip/core/imagecollection.h>
#include <fitsip/core/fitsobject.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

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

std::vector<std::shared_ptr<FitsObject>> OpCombineChannels::getCreatedImages() const
{
  return std::vector<std::shared_ptr<FitsObject>>{std::make_shared<FitsObject>(img)};
}

QString OpCombineChannels::getMenuEntry() const
{
  return "Image/Color/Combine...";
}

#ifdef USE_PYTHON
void OpCombineChannels::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
  m->def("combine",[this](std::shared_ptr<FitsObject> rimg, std::shared_ptr<FitsObject> gimg, std::shared_ptr<FitsObject> bimg){
    auto img = combine(rimg,gimg,bimg);
    return std::make_shared<FitsObject>(img);
  },
  "Combine RGB channels",py::arg("rimg"),py::arg("gimg"),py::arg("bimg"));
}
#endif

OpPlugin::ResultType OpCombineChannels::execute(std::shared_ptr<FitsObject> /*image*/, const OpPluginData& data)
{
  ImageCollection collection;
  for (const auto& img : data.imageCollection->getFiles())
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
    img = combine(collection.getFile(dlg->getRedImage()),collection.getFile(dlg->getGreenImage()),collection.getFile(dlg->getBlueImage()));
    if (!img)
    {
      const char* msg = "Images are not compatible";
      setError(msg);
      qWarning() << msg;
      return ERROR;
    }
  }
  return OK;
}

std::shared_ptr<FitsImage> OpCombineChannels::combine(std::shared_ptr<FitsObject> rimg, std::shared_ptr<FitsObject> gimg, std::shared_ptr<FitsObject> bimg) const
{
  auto r = rimg->getImage();
  auto g = gimg->getImage();
  auto b = bimg->getImage();
  if (r->isCompatible(*g.get()) && r->isCompatible(*b.get()))
  {
    auto img = std::make_shared<FitsImage>(r->getName()+"_RGB",r->getWidth(),r->getHeight(),3);
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
    return img;
  }
  return std::shared_ptr<FitsImage>();
}


