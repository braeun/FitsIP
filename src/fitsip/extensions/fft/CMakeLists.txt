
add_library(fitsfft SHARED
  fftplugincollection.h fftplugincollection.cpp
  opfft.cpp opfft.h
  opfftconvolution.cpp opfftconvolution.h
  opfftconvolutiondialog.cpp opfftconvolutiondialog.h opfftconvolutiondialog.ui
  opinvfft.cpp opinvfft.h
  plugin.json
)

target_include_directories(fitsfft
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsfft PRIVATE ${FITS_DEF})

target_compile_options(fitsfft PRIVATE -fPIC)

target_link_libraries(fitsfft
  PUBLIC fitsip::core
  PUBLIC PkgConfig::FFTW
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitsfft PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsfft
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitsfft
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

