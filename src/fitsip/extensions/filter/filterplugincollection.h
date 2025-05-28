#ifndef FILTERPLUGINCOLLECTION_H
#define FILTERPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

class FilterPluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  FilterPluginCollection();
  ~FilterPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};

#endif // FILTERPLUGINCOLLECTION_H
