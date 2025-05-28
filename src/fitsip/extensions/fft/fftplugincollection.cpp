#include "fftplugincollection.h"
#include "opfft.h"
#include "opfftconvolution.h"
#include "opinvfft.h"

FFTPluginCollection::FFTPluginCollection()
{
  plugins.push_back(new OpFFT());
  plugins.push_back(new OpFFTConvolution());
  plugins.push_back(new OpInvFFT());
}

FFTPluginCollection::~FFTPluginCollection()
{
}

std::vector<OpPlugin*> FFTPluginCollection::getPlugins() const
{
  return plugins;
}
