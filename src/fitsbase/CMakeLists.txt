
set(private_header_list
  externaltoolslauncher.h
  filelist.h
  fitsimage.h
  fitsobject.h
  fitstypes.h
  histogram.h
  imagecollection.h
  imagemetadata.h
  imagestatistics.h
  kernel.h
  kernelrepository.h
  opplugin.h
  pixel.h
  pixeliterator.h
  pixellist.h
  plugin.h
  pluginfactory.h
  profile.h
  rgbvalue.h
  scriptutilities.h
  settings.h
  star.h
  starlist.h
  undostack.h
  dialogs/imageselectdialog.h
  dialogs/imageselectwidget.h
  dialogs/pluginfilelistreturndialog.h
  dialogs/progressdialog.h
  dialogs/psfdialog.h
  dialogs/textinfodialog.h
  dialogs/twovaluedialog.h
  io/astroimageio.h
  io/cookbookio.h
  io/db.h
  io/fitsio.h
  io/iofactory.h
  io/iohandler.h
  io/qtimageio.h
  logbook/logbook.h
  logbook/logbookentry.h
  logbook/logbookfilter.h
  logbook/logbookstorage.h
  logbook/xmllogbookstorage.h
  math/average.h
  math/fitpar.h
  math/gaussfit.h
  math/mathfunctions.h
  math/moments.h
  math/filter/chebyshevfilter.h
  math/filter/filter.h
  math/filter/iirfilter.h
  math/filter/simplebandpassfilter.h
  math/filter/simplehighpassfilter.h
  math/filter/simplelowpassfilter.h
  psf/cosinebellpsf.h
  psf/gaussianpsf.h
  psf/imagepsf.h
  psf/psf.h
  psf/psffactory.h
  profiling/profilerentry.h
  profiling/simpleprofiler.h
  qconsolewidget/qconsoleiodevice.h
  qconsolewidget/qconsolewidget.h
  qxt/qxtglobal.h
#  qxt/qxtnamespace.h
  qxt/qxtspanslider.h
  qxt/qxtspanslider_p.h
  widgets/previewoptions.h
  widgets/previewwidget.h
  widgets/profilechart.h
  widgets/psfwidget.h
  )

add_library(fitsbase
  externaltoolslauncher.cpp
  filelist.cpp
  fitsimage.cpp
  fitsobject.cpp
  histogram.cpp
  imagecollection.cpp
  imagemetadata.cpp
  imagestatistics.cpp
  kernel.cpp
  kernelrepository.cpp
  opplugin.cpp
  pixeliterator.cpp
  pixellist.cpp
  plugin.cpp
  pluginfactory.cpp
  profile.cpp
  rgbvalue.cpp
  scriptutilities.cpp
  settings.cpp
  starlist.cpp
  undostack.cpp
  dialogs/imageselectdialog.cpp
  dialogs/imageselectdialog.ui
  dialogs/imageselectwidget.cpp
  dialogs/imageselectwidget.ui
  dialogs/pluginfilelistreturndialog.cpp dialogs/pluginfilelistreturndialog.ui
  dialogs/progressdialog.cpp
  dialogs/progressdialog.ui
  dialogs/psfdialog.cpp
  dialogs/psfdialog.ui
  dialogs/textinfodialog.cpp
  dialogs/textinfodialog.ui
  dialogs/twovaluedialog.cpp
  dialogs/twovaluedialog.ui
  io/astroimageio.cpp
  io/cookbookio.cpp
  io/db.cpp
  io/fitsio.cpp
  io/iofactory.cpp
  io/iohandler.cpp
  io/qtimageio.cpp
  logbook/logbook.cpp
  logbook/logbookentry.cpp
  logbook/logbookfilter.cpp
  logbook/logbookstorage.cpp
  logbook/xmllogbookstorage.cpp
  math/average.cpp
  math/fitpar.cpp
  math/gaussfit.cpp
  math/mathfunctions.cpp
  math/moments.cpp
  math/filter/chebyshevfilter.cpp
  math/filter/filter.cpp
  math/filter/iirfilter.cpp
  math/filter/simplebandpassfilter.cpp
  math/filter/simplehighpassfilter.cpp
  math/filter/simplelowpassfilter.cpp
  psf/cosinebellpsf.cpp
  psf/gaussianpsf.cpp
  psf/imagepsf.cpp
  psf/psf.cpp
  psf/psffactory.cpp
  profiling/profilerentry.cpp
  profiling/simpleprofiler.cpp
  qconsolewidget/qconsoleiodevice.cpp
  qconsolewidget/qconsolewidget.cpp
  qxt/qxtglobal.cpp
  qxt/qxtspanslider.cpp
  widgets/previewwidget.cpp widgets/previewwidget.ui
  widgets/previewoptions.cpp
  widgets/profilechart.cpp
  widgets/psfwidget.cpp widgets/psfwidget.ui
  ${private_header_list}
  )

add_library(fitsip::fitsbase ALIAS fitsbase)

if (LIBRAW_FOUND)
  target_sources(fitsbase
    PRIVATE io/rawio.cpp
    PRIVATE io/rawio.h
    )
endif()

target_include_directories(fitsbase PRIVATE ${CFITSIO_INCLUDE_DIR})

target_compile_definitions(fitsbase PRIVATE ${FITS_DEF})

target_compile_options(fitsbase PRIVATE -fPIC)

target_link_libraries(fitsbase
  PUBLIC PkgConfig::CCfits
  PUBLIC PkgConfig::cfitsio
  PUBLIC PkgConfig::QWT
  PUBLIC Qt5::Xml
  PUBLIC Qt5::Sql
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
#   # required by pybind11
#   # maybe just set the class with __attribute__ ((visibility("hidden")))
#   set_target_properties(fitsbase PROPERTIES CXX_VISIBILITY_PRESET hidden)
#   target_sources(fitsbase
#     PRIVATE pythonbase.h pythonbase.cpp
#   )
  # target_link_libraries(fitsbase
  #   PRIVATE pybind11::embed
  # )
endif()
