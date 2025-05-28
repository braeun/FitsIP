#ifndef MATCHINGPLUGINCOLLECTION_H
#define MATCHINGPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

class MatchingPluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  MatchingPluginCollection();
  ~MatchingPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};

#endif // MATCHINGPLUGINCOLLECTION_H
