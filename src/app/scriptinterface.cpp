#include "scriptinterface.h"
#include <fitsbase/imagecollection.h>
#include <fitsbase/io/iofactory.h>
#include <QFileInfo>
#include <QDebug>

#ifdef USE_PYTHON
#include <pybind11/embed.h>
#endif

std::unique_ptr<ScriptInterface> ScriptInterface::interface;

ScriptInterface::ScriptInterface():QObject()
{
}

void ScriptInterface::setWorkingDir(const QString& dir)
{
  workingdir = dir;
}

std::shared_ptr<FitsObject> ScriptInterface::load(const std::string& filename)
{
  QFileInfo fileinfo(QString::fromStdString(filename));
  if (!fileinfo.isAbsolute())
  {
    fileinfo = QFileInfo(workingdir+"/"+QString::fromStdString(filename));
  }
  std::shared_ptr<FitsObject> loaded = ImageCollection::getGlobal().getFile(fileinfo.absoluteFilePath());
  if (loaded)
  {
    return loaded;
  }
  IOHandler* handler = IOFactory::getInstance()->getHandler(fileinfo.absoluteFilePath());
  if (handler)
  {
    try
    {
      std::shared_ptr<FitsImage> image = handler->read(fileinfo.absoluteFilePath());
      std::shared_ptr<FitsObject> file = std::make_shared<FitsObject>(image,fileinfo.absoluteFilePath());
      ImageCollection::getGlobal().addFile(file);
      return file;
    }
    catch (std::exception& ex)
    {
      qCritical() << ex.what();
    }
  }
  else
  {
    qWarning() << "No IOHandler for " << fileinfo.absoluteFilePath();
  }
  return std::shared_ptr<FitsObject>();
}


#ifdef USE_PYTHON

void ScriptInterface::bind(py::module_& m)
{
  std::cout << "ScriptInterface::bind" << std::endl;
  m.def("load",[](const std::string& fn){return ScriptInterface::getInterface()->load(fn);});
  m.attr("a") = 1;
}

#endif

ScriptInterface* ScriptInterface::getInterface()
{
  if (!interface)
  {
    interface.reset(new ScriptInterface());
  }
  return interface.get();
}
