#include "opfftconvolution.h"
#include "opfftconvolutiondialog.h"
#include "opfft.h"
#include "opinvfft.h"
#include <fitsip/core/psf/psffactory.h>

#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif

OpFFTConvolution::OpFFTConvolution():
  dlg(nullptr)
{
  profiler = SimpleProfiler("OpFFTConvolution");
}

OpFFTConvolution::~OpFFTConvolution()
{
}

QString OpFFTConvolution::getMenuEntry() const
{
  return "Filter/FFT Convolution...";
}

#ifdef USE_PYTHON
void OpFFTConvolution::bindPython(void* mod) const
{
  py::module_* m = reinterpret_cast<py::module_*>(mod);
//  m->def("fft",[this](std::shared_ptr<FitsObject> obj){
//    auto img = fft(*obj->getImage());
//    return std::make_shared<FitsObject>(img);
//  },
//  "Calculate FFT of the image",py::arg("obj"));
}
#endif

OpPlugin::ResultType OpFFTConvolution::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (!dlg) dlg = new OpFFTConvolutionDialog();
  dlg->updatePSFList();
  if (dlg->exec())
  {
    const PSF* psf = PSFFactory::getInstance()->getPSF(dlg->getFunction());
    auto psfpar = dlg->getParameters();
    if (psf)
    {
      try
      {
        profiler.start();
        auto img = fftconvolution(image->getImage().get(),psf,psfpar);
        image->setImage(img);
        profiler.stop();
        QString msg = "FFT deconvolution: ";
        msg += psf->getName();
        if (!psfpar.empty())
        {
          msg += " par=";
          for (size_t i=0;i<psfpar.size();++i)
          {
            if (i > 0) msg += ",";
            msg += QString::asprintf("%.1f",psfpar[i]);
          }
        }
        log(image,msg);
        logProfiler(image);
        return OK;
      }
      catch (std::runtime_error& ex)
      {
        setError(ex.what());
        return ERROR;
      }
    }
    else
    {
      setError("Internal Error: PSF '"+dlg->getFunction()+"' not found!");
      return ERROR;
    }
    return OK;
  }
  return CANCELLED;
}

std::shared_ptr<FitsImage> OpFFTConvolution::fftconvolution(FitsImage* img, const PSF* psf, const std::vector<ValueType>& par) const
{
  auto h = psf->createPSF(img->getWidth(),img->getHeight(),par);
  auto psffft = OpFFT().fft(*h);
  auto imgfft = OpFFT().fft(*img);
  *imgfft *= *psffft;
  auto cimg = OpInvFFT().invfft(imgfft);
  return cimg;
}


