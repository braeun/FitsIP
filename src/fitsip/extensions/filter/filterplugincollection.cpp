#include "filterplugincollection.h"
#include "opddp.h"
#include "opgaussblur.h"
#include "opkernel.h"
#include "opmedian.h"
#include "opsobel.h"
#include "opunsharpmask.h"

FilterPluginCollection::FilterPluginCollection()
{
//  Q_INIT_RESOURCE(pluginresources);
  plugins.push_back(new OpDDP());
  plugins.push_back(new OpGaussBlur());
  plugins.push_back(new OpKernel());
  plugins.push_back(new OpMedian());
  plugins.push_back(new OpSobel());
  plugins.push_back(new OpUnsharpMask());
}

FilterPluginCollection::~FilterPluginCollection()
{
  // for (OpPlugin* p : plugins) delete p;
  // plugins.clear();
}

std::vector<OpPlugin*> FilterPluginCollection::getPlugins() const
{
  return plugins;
}



