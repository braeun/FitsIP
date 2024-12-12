#include "opcombinechannels.h"
#include "opcombinechannelsdialog.h"
#include <imagecollection.h>
#include <fileobject.h>

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

OpPlugin::ResultType OpCombineChannels::execute(std::shared_ptr<FitsImage> /*image*/, QRect selection)
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
