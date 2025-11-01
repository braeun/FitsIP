#include "testplugincollection.h"
#include "boxtestimage.h"
#include "gaussiantestimage.h"
#include "moffattestimage.h"
#include "psftestimage.h"
#include "rulerblackonwhitetestimage.h"
#include "rulerwhiteonblacktestimage.h"

TestPluginCollection::TestPluginCollection()
{
  plugins.push_back(new BoxTestImage());
  plugins.push_back(new GaussianTestImage());
  plugins.push_back(new MoffatTestImage());
  plugins.push_back(new PSFTestImage());
  plugins.push_back(new RulerBlackOnWhiteTestImage());
  plugins.push_back(new RulerWhiteOnBlackTestImage());
}

TestPluginCollection::~TestPluginCollection()
{
}

std::vector<OpPlugin*> TestPluginCollection::getPlugins() const
{
  return plugins;
}



