# Fits - core plugins
#
# modified: 2025-05-25

add_subdirectory(standard)
add_subdirectory(test)

#
# The core plugins are always available in Fits. Thus they are compiled into
# a static library and linked to the application. Because the moc file
# of each plugin declares a static variable of the same name, the moc files
# cannot be in the same compilation unit. Thus we cannot use automoc, as
# it includes all moc files in a single compilation unit.
#

set(plugin_headers
  analyseprofile.h
  findstars.h
  measurecrosscorrelation.h
  measuredistance.h
  measurematch.h
#  measurerank.h
  measuresharpness.h
  measurestatistics.h
  opalign.h
  opaverage.h
  opcalibration.h
  opcombinechannels.h
  opddp.h
  opgaussblur.h
#  opgrow.h
  opkernel.h
  opmedian.h
#  opshrink.h
  opsobel.h
  opsplitchannels.h
  opstack.h
  optogray.h
  opunsharpmask.h
  psfimage.h
  synthesizebackground.h
  )

set(plugin_sources
  analyseprofile.cpp
  findstars.cpp
  measurecrosscorrelation.cpp
  measuredistance.cpp
  measurematch.cpp
#  measurerank.cpp
  measuresharpness.cpp
  measurestatistics.cpp
  opalign.cpp
  opaverage.cpp
  opcalibration.cpp
  opcombinechannels.cpp
  opddp.cpp
  opgaussblur.cpp
#  opgrow.cpp
  opkernel.cpp
  opmedian.cpp
#  opshrink.cpp
  opsobel.cpp
  opsplitchannels.cpp
  opstack.cpp
  optogray.cpp
  opunsharpmask.cpp
  psfimage.cpp
  synthesizebackground.cpp
  )

if (FFTW_FOUND)
  list(APPEND plugin_headers
    lucyrichardsondeconvolution.h
    opfft.h
    opfftconvolution.h
    opinvfft.h
    vancittertdeconvolution.h
    )
  list(APPEND plugin_sources
    lucyrichardsondeconvolution.cpp
    opfft.cpp
    opfftconvolution.cpp
    opinvfft.cpp
    vancittertdeconvolution.cpp
    )
endif()

foreach(header plugin_headers)
  set_property(SOURCE ${header} PROPERTY SKIP_AUTOMOC ON)
endforeach()

qt5_wrap_cpp(plugin_mocs
  ${plugin_headers}
  OPTIONS -I${PROJECT_SOURCE_DIR}/src
  )

set(private_header_list
  analyseprofiledialog.h
  findstarsdialog.h
  lucyrichardsondeconvolutiondialog.h
  measuredistancedialog.h
  measurematchdialog.h
#  measurerankresultdialog.h
  measuresharpnessresultdialog.h
  opaligndialog.h
  opcalibrationdialog.h
  opcombinechannelsdialog.h
  opddpdialog.h
  opfftconvolutiondialog.h
  opgaussblurdialog.h
  opkerneldialog.h
  opmediandialog.h
  opstackdialog.h
  opunsharpmaskdialog.h
  synthesizebackgrounddialog.h
  vancittertdeconvolutiondialog.h
)

add_library(fitsplugins
  analyseprofiledialog.cpp analyseprofiledialog.ui
  findstarsdialog.cpp findstarsdialog.ui
  lucyrichardsondeconvolutiondialog.cpp lucyrichardsondeconvolutiondialog.ui
  measuredistancedialog.cpp measuredistancedialog.ui
  measurematchdialog.cpp measurematchdialog.ui
#  measurerankresultdialog.cpp measurerankresultdialog.ui
  measuresharpnessresultdialog.cpp measuresharpnessresultdialog.ui
  opaligndialog.cpp opaligndialog.ui
  opcalibrationdialog.cpp opcalibrationdialog.ui
  opcombinechannelsdialog.cpp opcombinechannelsdialog.ui
  opddpdialog.cpp opddpdialog.ui
  opfftconvolutiondialog.cpp opfftconvolutiondialog.ui
  opgaussblurdialog.cpp opgaussblurdialog.ui
#  opgrowdialog.cpp opgrowdialog.ui
  opkerneldialog.cpp opkerneldialog.ui
  opmediandialog.cpp opmediandialog.ui
#  opshrinkdialog.cpp opshrinkdialog.ui
  opstackdialog.cpp opstackdialog.ui
  opunsharpmaskdialog.cpp opunsharpmaskdialog.ui
  pluginresources.qrc
  synthesizebackgrounddialog.cpp synthesizebackgrounddialog.ui
  vancittertdeconvolutiondialog.cpp vancittertdeconvolutiondialog.ui
  ${plugin_sources}
  ${plugin_mocs}
  ${private_header_list}
  ${plugin_headers}
  )

target_include_directories(fitsplugins
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsplugins PRIVATE ${FITS_DEF})

target_compile_options(fitsplugins PRIVATE -fPIC)

if (FFTW_FOUND)
  target_link_libraries(fitsplugins
    PUBLIC PkgConfig::FFTW
    )
endif()

target_link_libraries(fitsplugins
  PUBLIC fitsbase
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
  set_target_properties(fitsplugins PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsplugins
    PRIVATE pybind11::embed
  )
endif()
