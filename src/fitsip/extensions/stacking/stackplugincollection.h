#ifndef STACKPLUGINCOLLECTION_H
#define STACKPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

class StackPluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  StackPluginCollection();
  ~StackPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};


#endif // STACKPLUGINCOLLECTION_H
