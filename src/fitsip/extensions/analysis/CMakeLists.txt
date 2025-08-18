
add_library(fitsanalysis SHARED
  analysisplugincollection.h analysisplugincollection.cpp
  analyseprofile.h analyseprofile.cpp
  analyseprofiledialog.h analyseprofiledialog.cpp analyseprofiledialog.ui
  measuredistance.h measuredistance.cpp
  measuredistancedialog.h measuredistancedialog.cpp measuredistancedialog.ui
  # measurerank.h measurerank.cpp
  # measurerankresultdialog.h measurerankresultdialog.cpp measurerankresultdialog.ui
  measuresharpness.h measuresharpness.cpp
  measuresharpnessresultdialog.cpp measuresharpnessresultdialog.h measuresharpnessresultdialog.ui
  measurestatistics.h measurestatistics.cpp
  s3sharpness.h s3sharpness.cpp
  s3sharpnessresultdialog.h s3sharpnessresultdialog.cpp s3sharpnessresultdialog.ui
  synthesizebackground.cpp synthesizebackground.h
  synthesizebackgrounddialog.cpp synthesizebackgrounddialog.h synthesizebackgrounddialog.ui
  plugin.json
)

target_include_directories(fitsanalysis
PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsanalysis PRIVATE ${FITS_DEF})

target_compile_options(fitsanalysis PRIVATE -fPIC)

target_link_libraries(fitsanalysis
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (FFTW_FOUND)
  target_sources(fitsanalysis PRIVATE
    measurecrosscorrelation.h measurecrosscorrelation.cpp
  )
  target_link_libraries(fitsanalysis
    PUBLIC PkgConfig::FFTW
  )
endif()

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitstestimages PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsanalysis
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitsanalysis
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

