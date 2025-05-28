#include "matchingplugincollection.h"
#include "findstars.h"
#include "measurematch.h"

MatchingPluginCollection::MatchingPluginCollection()
{
  plugins.push_back(new FindStars());
  plugins.push_back(new MeasureMatch());
}

MatchingPluginCollection::~MatchingPluginCollection()
{
  // for (OpPlugin* p : plugins) delete p;
  // plugins.clear();
}

std::vector<OpPlugin*> MatchingPluginCollection::getPlugins() const
{
  return plugins;
}



