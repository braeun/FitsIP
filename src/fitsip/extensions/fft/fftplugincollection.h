#ifndef FFTPLUGINCOLLECTION_H
#define FFTPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

class FFTPluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  FFTPluginCollection();
  ~FFTPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};

#endif // FFTPLUGINCOLLECTION_H
