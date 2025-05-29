#include "opfftconvolution.h"
#include "opfftconvolutiondialog.h"
#include <fitsip/core/psf/psffactory.h>
#include <fftw3.h>

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
  int fftsize = img->getWidth() * (img->getHeight() / 2 + 1);
  auto h = psf->createPSF(img->getWidth(),img->getHeight(),par);
  fftw_complex *cout = new fftw_complex[fftsize];
  double *rin = new double[img->getHeight()*img->getWidth()];
  fftw_plan r2c = fftw_plan_dft_r2c_2d(img->getHeight(),img->getWidth(),rin,cout,FFTW_ESTIMATE);
  {
    ConstPixelIterator it = h->getConstPixelIterator();
    double* ptr = rin;
    for (int i=0;i<img->getHeight()*img->getWidth();i++)
    {
      *ptr++ = it.getAbs();
      ++it;
    }
    fftw_execute(r2c);
  }
  fftw_complex *ch = new fftw_complex[fftsize];
  memcpy(ch,cout,fftsize*sizeof(fftw_complex));
  {
    ConstPixelIterator it = img->getConstPixelIterator();
    double* ptr = rin;
    for (int i=0;i<img->getHeight()*img->getWidth();i++)
    {
      *ptr++ = it.getAbs();
      ++it;
    }
    fftw_execute(r2c);
  }
  fftw_destroy_plan(r2c);
  mul(ch,cout,fftsize);
  fftw_plan c2r = fftw_plan_dft_c2r_2d(img->getHeight(),img->getWidth(),cout,rin,FFTW_ESTIMATE);
  memcpy(cout,ch,fftsize*sizeof(fftw_complex));
  fftw_execute(c2r);
  auto fftimg = std::make_shared<FitsImage>("tmp",img->getWidth(),img->getHeight(),1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = rin;
  for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
  {
    it2[0] = *ptr;
    ++ptr;
    ++it2;
  }
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  fftw_destroy_plan(c2r);
  delete [] rin;
  delete [] cout;
  delete [] ch;
  return fftimg;
}

void OpFFTConvolution::mul(fftw_complex *a, fftw_complex *b, int n) const
{
  while (n-- > 0)
  {
    double re = (*a)[0] * (*b)[0] - (*a)[1] * (*b)[1];
    double im = (*a)[0] * (*b)[1] + (*a)[1] * (*b)[0];
    (*a)[0] = re;
    (*a)[1] = im;
    ++a;
    ++b;
  }
}


