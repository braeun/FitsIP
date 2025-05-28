#ifndef TESTPLUGINCOLLECTION_H
#define TESTPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

class TestPluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  TestPluginCollection();
  ~TestPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};

#endif // TESTPLUGINCOLLECTION_H
