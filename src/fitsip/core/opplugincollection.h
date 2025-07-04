#ifndef OPPLUGINCOLLECTION_H
#define OPPLUGINCOLLECTION_H

#include <QObject>
#include <vector>

class OpPlugin;

class OpPluginCollection: public QObject
{
  Q_OBJECT
public:
  OpPluginCollection();

  virtual std::vector<OpPlugin*> getPlugins() const = 0;
};

#define OpPluginCollection_iid "net.hbr.OpPluginCollection/1.0"

Q_DECLARE_INTERFACE(OpPluginCollection,OpPluginCollection_iid);

#endif // OPPLUGINCOLLECTION_H
