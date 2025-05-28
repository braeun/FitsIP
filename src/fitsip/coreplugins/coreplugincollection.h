#ifndef STANDARDPLUGINCOLLECTION_H
#define STANDARDPLUGINCOLLECTION_H

#include <fitsip/core/opplugincollection.h>

// #define QT_STATICPLUGIN
// #include <QtPlugin>

class CorePluginCollection: public OpPluginCollection
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPluginCollection_iid FILE "plugin.json")
  Q_INTERFACES(OpPluginCollection)
public:
  CorePluginCollection();
  ~CorePluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const override;

private:
  std::vector<OpPlugin*> plugins;
};

#endif // STANDARDPLUGINCOLLECTION_H
