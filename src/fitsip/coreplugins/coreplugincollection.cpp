#include "coreplugincollection.h"
#include "opaverage.h"
#include "opadd.h"
#include "opcombinechannels.h"
#include "opcrop.h"
#include "opcut.h"
#include "opdiv.h"
#include "opflipx.h"
#include "opflipy.h"
#include "oplog.h"
#include "opmul.h"
#include "opresize.h"
#include "oprotate.h"
#include "opscale.h"
#include "opshift.h"
#include "opsplitchannels.h"
#include "opsqrt.h"
#include "opsub.h"
#include "optogray.h"


CorePluginCollection::CorePluginCollection()
{
  plugins.push_back(new OpFlipX());
  plugins.push_back(new OpFlipY());
  plugins.push_back(new OpResize());
  plugins.push_back(new OpRotate());
  plugins.push_back(new OpScale());
  plugins.push_back(new OpShift());
  plugins.push_back(new OpAdd());
  plugins.push_back(new OpCrop());
  plugins.push_back(new OpCut());
  plugins.push_back(new OpDiv());
  plugins.push_back(new OpLog());
  plugins.push_back(new OpMul());
  plugins.push_back(new OpSqrt());
  plugins.push_back(new OpSub());
  plugins.push_back(new OpCombineChannels());
  plugins.push_back(new OpSplitChannels());
  plugins.push_back(new OpToGray());
  plugins.push_back(new OpAverage());
}

CorePluginCollection::~CorePluginCollection()
{
  // for (OpPlugin* p : plugins) delete p;
  // plugins.clear();
}

std::vector<OpPlugin*> CorePluginCollection::getPlugins() const
{
  return plugins;
}



