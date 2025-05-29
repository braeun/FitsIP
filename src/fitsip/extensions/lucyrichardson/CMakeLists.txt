# Fits - vanCittert deconvolution plugin
#
# modified: 2025-05-28


add_library(fitslucyrichardson SHARED
  lucyrichardsondeconvolution.cpp lucyrichardsondeconvolution.h
  lucyrichardsondeconvolutiondialog.cpp lucyrichardsondeconvolutiondialog.h lucyrichardsondeconvolutiondialog.ui
  plugin.json
  lrresources.qrc
  )

target_include_directories(fitslucyrichardson
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitslucyrichardson PRIVATE ${FITS_DEF})

target_compile_options(fitslucyrichardson PRIVATE -fPIC)

target_link_libraries(fitslucyrichardson
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  PUBLIC PkgConfig::FFTW
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitslucyrichardson PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitslucyrichardson
    PRIVATE pybind11::embed
  )
endif()
