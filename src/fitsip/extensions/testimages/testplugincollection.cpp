#include "testplugincollection.h"
#include "boxtestimage.h"
#include "gaussiantestimage.h"
#include "psftestimage.h"

TestPluginCollection::TestPluginCollection()
{
  plugins.push_back(new BoxTestImage());
  plugins.push_back(new GaussianTestImage());
  plugins.push_back(new PSFTestImage());
}

TestPluginCollection::~TestPluginCollection()
{
  // for (OpPlugin* p : plugins) delete p;
  // plugins.clear();
}

std::vector<OpPlugin*> TestPluginCollection::getPlugins() const
{
  return plugins;
}



