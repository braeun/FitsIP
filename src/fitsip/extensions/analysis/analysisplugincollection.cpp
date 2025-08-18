#include "analysisplugincollection.h"
#include "analyseprofile.h"
#include "measurecrosscorrelation.h"
#include "measuredistance.h"
#include "measuresharpness.h"
#include "measurestatistics.h"
#include "s3sharpness.h"
#include "synthesizebackground.h"

AnalysisPluginCollection::AnalysisPluginCollection()
{
  plugins.push_back(new AnalyseProfile());
  plugins.push_back(new MeasureCrossCorrelation());
  plugins.push_back(new MeasureDistance());
  plugins.push_back(new MeasureSharpness());
  plugins.push_back(new MeasureStatistics());
  plugins.push_back(new S3Sharpness());
  plugins.push_back(new SynthesizeBackground());
}

AnalysisPluginCollection::~AnalysisPluginCollection()
{
}

std::vector<OpPlugin*> AnalysisPluginCollection::getPlugins() const
{
  return plugins;
}
