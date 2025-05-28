#ifndef ANALYSISPLUGINCOLLECTION_H
#define ANALYSISPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

class AnalysisPluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  AnalysisPluginCollection();
  ~AnalysisPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};

#endif // ANALYSISPLUGINCOLLECTION_H
