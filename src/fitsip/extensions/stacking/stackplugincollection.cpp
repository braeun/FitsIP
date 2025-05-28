#include "stackplugincollection.h"
#include "opalign.h"
#include "opcalibration.h"
#include "opstack.h"

StackPluginCollection::StackPluginCollection()
{
  plugins.push_back(new OpAlign());
  plugins.push_back(new OpCalibration());
  plugins.push_back(new OpStack());
}

StackPluginCollection::~StackPluginCollection()
{
  // for (OpPlugin* p : plugins) delete p;
  // plugins.clear();
}

std::vector<OpPlugin*> StackPluginCollection::getPlugins() const
{
  return plugins;
}



