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
        auto img = fftconvolution(image->getImage(),psf,psfpar);
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
  fftdata data;
  int w0 = img->getWidth() + psf->getWidth();
  w0 += w0 % 2;
  int h0 = img->getHeight() + psf->getHeight();
  data.fftsize = w0 * (h0 / 2 + 1);
  auto h = psf->createPSF(w0,h0,par);
  data.cinout = new fftw_complex[data.fftsize];
  data.rinout = new double[h0*w0];
  data.r2c = fftw_plan_dft_r2c_2d(h0,w0,data.rinout,data.cinout,FFTW_ESTIMATE);
  data.c2r = fftw_plan_dft_c2r_2d(h0,w0,data.cinout,data.rinout,FFTW_ESTIMATE);
  fft(data,h,0);
  fftw_complex* hfft = new fftw_complex[data.fftsize];
  memcpy(hfft,data.cinout,data.fftsize*sizeof(fftw_complex));
  auto imgpadded = img->paddedImage(w0,h0); //std::make_shared<FitsImage>(*image);
  std::shared_ptr<FitsImage> fftimage;
  if (imgpadded.getDepth() == 1)
  {
    fft(data,imgpadded,0);
    mul(data.cinout,hfft,data.fftsize);
    fftimage = invfft(data,data.cinout,w0,h0);
  }
  else if (imgpadded.getDepth() == 3)
  {
    fftw_complex* offt1 = new fftw_complex[data.fftsize];
    fftw_complex* offt2 = new fftw_complex[data.fftsize];
    fftw_complex* offt3 = new fftw_complex[data.fftsize];
    fft(data,imgpadded,0);
    memcpy(offt1,data.cinout,data.fftsize*sizeof(fftw_complex));
    fft(data,imgpadded,1);
    memcpy(offt2,data.cinout,data.fftsize*sizeof(fftw_complex));
    fft(data,imgpadded,2);
    memcpy(offt3,data.cinout,data.fftsize*sizeof(fftw_complex));
    mul(offt1,hfft,data.fftsize);
    mul(offt2,hfft,data.fftsize);
    mul(offt3,hfft,data.fftsize);
    fftimage = invfft(data,offt1,offt2,offt3,w0,h0);
    delete [] offt1;
    delete [] offt2;
    delete [] offt3;
  }
  fftw_destroy_plan(data.r2c);
  fftw_destroy_plan(data.c2r);
  delete [] data.rinout;
  delete [] data.cinout;
  delete [] hfft;
  /* crop to original size */
  fftimage = std::make_shared<FitsImage>(fftimage->subImage(QRect(0,0,img->getWidth(),img->getHeight())));
  return fftimage;
}

void OpFFTConvolution::fft(const fftdata& data, const FitsImage &image, int channel) const
{
  ConstPixelIterator it = image.getConstPixelIterator();
  double* ptr = data.rinout;
  for (int i=0;i<image.getHeight()*image.getWidth();i++)
  {
    *ptr++ = it[channel];
    ++it;
  }
  fftw_execute(data.r2c);
}

std::shared_ptr<FitsImage> OpFFTConvolution::invfft(const fftdata& data, fftw_complex* c, int w, int h) const
{
  memmove(data.cinout,c,data.fftsize*sizeof(fftw_complex));
  fftw_execute(data.c2r);
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,1);
  PixelIterator it2 = fftimg->getPixelIterator();
  double* ptr = data.rinout;
  for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
  {
    it2[0] = *ptr;
    ++ptr;
    ++it2;
  }
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
  return fftimg;
}

std::shared_ptr<FitsImage> OpFFTConvolution::invfft(const fftdata& data, fftw_complex* c1, fftw_complex* c2, fftw_complex* c3, int w, int h) const
{
  auto fftimg = std::make_shared<FitsImage>("tmp",w,h,3);
  {
    memmove(data.cinout,c1,data.fftsize*sizeof(fftw_complex));
    fftw_execute(data.c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = data.rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[0] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  {
    memmove(data.cinout,c2,data.fftsize*sizeof(fftw_complex));
    fftw_execute(data.c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = data.rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[1] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  {
    memmove(data.cinout,c3,data.fftsize*sizeof(fftw_complex));
    fftw_execute(data.c2r);
    PixelIterator it2 = fftimg->getPixelIterator();
    double* ptr = data.rinout;
    for (int i=0;i<fftimg->getHeight()*fftimg->getWidth();i++)
    {
      it2[2] = *ptr;
      ++ptr;
      ++it2;
    }
  }
  *fftimg /= fftimg->getHeight() * fftimg->getWidth();
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


