# Fits - vanCittert deconvolution plugin
#
# modified: 2025-05-28


add_library(fitsvancittert SHARED
  vancittertdeconvolution.h vancittertdeconvolution.cpp
  vancittertdeconvolutiondialog.h vancittertdeconvolutiondialog.cpp vancittertdeconvolutiondialog.ui
  plugin.json
  vcresources.qrc
  )

target_include_directories(fitsvancittert
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsvancittert PRIVATE ${FITS_DEF})

target_compile_options(fitsvancittert PRIVATE -fPIC)

target_link_libraries(fitsvancittert
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  PUBLIC PkgConfig::FFTW
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitsvancittert PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsvancittert
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitsvancittert
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

