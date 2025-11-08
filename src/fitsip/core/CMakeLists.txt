
# generate the configuration file
configure_file(fitsconfig.h.in fitsconfig.h @ONLY)

add_library(fitscore SHARED
  annotation.cpp
  annotations.cpp
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
  opplugincollection.cpp
  pixeliterator.cpp
  pixellist.cpp
  plugin.cpp
  pluginfactory.cpp
  profile.cpp
  rgbvalue.cpp
  scriptutilities.cpp
  settings.cpp
  star.cpp
  starlist.cpp
  undostack.cpp
  xydata.cpp
  db/camera.cpp
  db/cameratablemodel.cpp
  db/database.cpp
  db/sqldatabase.cpp
  dialogs/imageselectdialog.cpp dialogs/imageselectdialog.ui
  dialogs/pluginfilelistreturndialog.cpp dialogs/pluginfilelistreturndialog.ui
  dialogs/plugininfodialog.cpp dialogs/plugininfodialog.ui
  dialogs/progressdialog.cpp dialogs/progressdialog.ui
  dialogs/psfdialog.cpp dialogs/psfdialog.ui
  dialogs/textinfodialog.cpp dialogs/textinfodialog.ui
  dialogs/twovaluedialog.cpp dialogs/twovaluedialog.ui
  io/astroimageio.cpp
  io/cookbookio.cpp
  io/fitsio.cpp
  io/iofactory.cpp
  io/iohandler.cpp
  io/qtimageio.cpp
  logbook/abstractlogbookstorage.cpp
  logbook/logbook.cpp
  logbook/logbookentry.cpp
  logbook/logbookfilter.cpp
  logbook/logbookstorage.cpp
  logbook/logbookutils.cpp
  logbook/xmllogbookstorage.cpp
  math/average.cpp
  math/fitpar.cpp
  math/gaussfit.cpp
  math/linearregression.cpp
  math/mathfunctions.cpp
  math/moments.cpp
  math/utils.cpp
  math/filter/chebyshevfilter.cpp
  math/filter/filter.cpp
  math/filter/iirfilter.cpp
  math/filter/simplebandpassfilter.cpp
  math/filter/simplehighpassfilter.cpp
  math/filter/simplelowpassfilter.cpp
  math/window/hanningwindow.cpp
  psf/cosinebellpsf.cpp
  psf/gaussianpsf.cpp
  psf/imagepsf.cpp
  psf/moffatpsf.cpp
  psf/psf.cpp
  psf/psffactory.cpp
  psf/psfmanagerdialog.cpp psf/psfmanagerdialog.ui
  profiling/profilerentry.cpp
  profiling/simpleprofiler.cpp
  qconsolewidget/qconsoleiodevice.cpp
  qconsolewidget/qconsolewidget.cpp
  qxt/qxtglobal.cpp
  qxt/qxtspanslider.cpp
  widgets/imageselectwidget.cpp widgets/imageselectwidget.ui
  widgets/previewwidget.cpp widgets/previewwidget.ui
  widgets/previewoptions.cpp
  widgets/profilechart.cpp
  widgets/psfwidget.cpp widgets/psfwidget.ui
  widgets/xychartswidget.cpp widgets/xychartswidget.ui
  )
add_library(fitsip::core ALIAS fitscore)

target_sources(fitscore PUBLIC
  FILE_SET HEADERS TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/../.." FILES
  annotation.h
  annotations.h
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
  opplugincollection.h
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
  db/camera.h
  db/cameratablemodel.h
  db/database.h
  db/sqldatabase.h
  dialogs/imageselectdialog.h
  dialogs/pluginfilelistreturndialog.h
  dialogs/plugininfodialog.h
  dialogs/progressdialog.h
  dialogs/psfdialog.h
  dialogs/textinfodialog.h
  dialogs/twovaluedialog.h
  io/astroimageio.h
  io/cookbookio.h
  io/fitsio.h
  io/iofactory.h
  io/iohandler.h
  io/qtimageio.h
  logbook/abstractlogbookstorage.h
  logbook/logbook.h
  logbook/logbookentry.h
  logbook/logbookfilter.h
  logbook/logbookstorage.h
  logbook/logbookutils.h
  logbook/xmllogbookstorage.h
  math/average.h
  math/fitpar.h
  math/gaussfit.h
  math/linearregression.h
  math/mathfunctions.h
  math/moments.h
  math/utils.h
  math/filter/chebyshevfilter.h
  math/filter/filter.h
  math/filter/iirfilter.h
  math/filter/simplebandpassfilter.h
  math/filter/simplehighpassfilter.h
  math/filter/simplelowpassfilter.h
  math/window/hanningwindow.h
  psf/cosinebellpsf.h
  psf/gaussianpsf.h
  psf/imagepsf.h
  psf/moffatpsf.h
  psf/psf.h
  psf/psffactory.h
  psf/psfmanagerdialog.h
  profiling/profilerentry.h
  profiling/simpleprofiler.h
  qconsolewidget/qconsoleiodevice.h
  qconsolewidget/qconsolewidget.h
  qxt/qxtglobal.h
  qxt/qxtspanslider.h
  qxt/qxtspanslider_p.h
  widgets/imageselectwidget.h
  widgets/previewoptions.h
  widgets/previewwidget.h
  widgets/profilechart.h
  widgets/psfwidget.h
  widgets/xychartswidget.h
  ${CMAKE_CURRENT_BINARY_DIR}/fitsconfig.h
  )

if (LIBRAW_FOUND)
  target_sources(fitscore
    PRIVATE io/rawio.cpp
    PUBLIC
    FILE_SET HEADERS TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../.." FILES io/rawio.h
    )
  target_link_libraries(fitscore
    PUBLIC PkgConfig::LIBRAW
  )
endif()

#target_include_directories(fitscore PRIVATE ${CFITSIO_INCLUDE_DIR})
target_include_directories(fitscore
PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitscore
  PRIVATE
    ${FITS_DEF}
    -DPLUGIN_DIR=${CMAKE_BINARY_DIR}/src/fitsip
    -DSHARE_DIR=${PROJECT_SOURCE_DIR}/share
  )

target_compile_options(fitscore PRIVATE -fPIC)

target_link_libraries(fitscore
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

if (nlohmann_json_FOUND)
  target_sources(fitscore PUBLIC
    db/jsondatabase.cpp
  )
  target_sources(fitscore PUBLIC
    FILE_SET HEADERS TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/../.." FILES
    db/jsondatabase.h
  )
  target_link_libraries(fitscore
    PUBLIC nlohmann_json::nlohmann_json
    )
endif()

set_target_properties(fitscore
    PROPERTIES
      VERSION
        ${CMAKE_PROJECT_VERSION}
      SOVERSION
        ${CMAKE_PROJECT_VERSION_MAJOR} #.${CMAKE_PROJECT_VERSION_MINOR}
      DEFINE_SYMBOL
        LIBIMCACORE
)

install(TARGETS fitscore
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

