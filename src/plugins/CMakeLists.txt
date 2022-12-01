# Fits - core plugins
#
# modified: 2022-01-27


#
# The core plugins are always available in Fits. Thus they are compiled into
# a static library and linked to the application. Because the moc file
# of each plugin declares a static variable of the same name, the moc files
# cannot be in the same compilation unit. Thus we cannot use automoc, as
# it includes all moc files in a single compilation unit.
#

set(plugin_headers
  findstars.h
  measurecrosscorrelation.h
  measurematch.h
  measurerank.h
  measurestatistics.h
  opadd.h
  opalign.h
  opaverage.h
  opcalibration.h
  opcrop.h
  opcut.h
  opddp.h
  opdiv.h
  opflipx.h
  opflipy.h
  opgaussblur.h
  opgrow.h
  opkernel.h
  oplog.h
  opmedian.h
  opmul.h
  oprotate.h
  opscale.h
  opshift.h
  opshrink.h
  opsobel.h
  opsplitchannels.h
  opsqrt.h
  opstack.h
  opsub.h
  optogray.h
  opunsharpmask.h
  psfimage.h
  synthesizebackground.h
  test/gaussiantestimage.h
  )

set(plugin_sources
  findstars.cpp
  measurecrosscorrelation.cpp
  measurematch.cpp
  measurerank.cpp
  measurestatistics.cpp
  opadd.cpp
  opalign.cpp
  opaverage.cpp
  opcalibration.cpp
  opcrop.cpp
  opcut.cpp
  opddp.cpp
  opdiv.cpp
  opflipx.cpp
  opflipy.cpp
  opgaussblur.cpp
  opgrow.cpp
  opkernel.cpp
  oplog.cpp
  opmedian.cpp
  opmul.cpp
  oprotate.cpp
  opscale.cpp
  opshift.cpp
  opshrink.cpp
  opsobel.cpp
  opsplitchannels.cpp
  opsqrt.cpp
  opstack.cpp
  opsub.cpp
  optogray.cpp
  opunsharpmask.cpp
  psfimage.cpp
  synthesizebackground.cpp
  test/gaussiantestimage.cpp
  )

if (FFTW_FOUND)
  list(APPEND plugin_headers
    opfft.h
    opinvfft.h
    vancittertdeconvolution.h
    )
  list(APPEND plugin_sources
    opfft.cpp
    opinvfft.cpp
    vancittertdeconvolution.cpp
    )
endif()

foreach(header plugin_headers)
  set_property(SOURCE ${header} PROPERTY SKIP_AUTOMOC ON)
endforeach()

qt5_wrap_cpp(plugin_mocs
  ${plugin_headers}
  OPTIONS -I${PROJECT_SOURCE_DIR}/src/lib
  )

set(private_header_list
  findstarsdialog.h
  measurematchdialog.h
  measurerankresultdialog.h
  opaligndialog.h
  opcalibrationdialog.h
  opcropdialog.h
  opddpdialog.h
  opgaussblurdialog.h
  opgrowdialog.h
  opkerneldialog.h
  opmediandialog.h
  oprotatedialog.h
  opshiftdialog.h
  opshrinkdialog.h
  opstackdialog.h
  opunsharpmaskdialog.h
  synthesizebackgrounddialog.h
  vancittertdeconvolutiondialog.h
  test/gaussiantestimagedialog.h
  )

add_library(fitsplugins
  findstarsdialog.cpp
  findstarsdialog.ui
  measurematchdialog.cpp
  measurematchdialog.ui
  measurerankresultdialog.cpp
  measurerankresultdialog.ui
  opaligndialog.cpp
  opaligndialog.ui
  opcalibrationdialog.cpp
  opcalibrationdialog.ui
  opcropdialog.cpp
  opcropdialog.ui
  opddpdialog.cpp
  opddpdialog.ui
  opgaussblurdialog.cpp
  opgaussblurdialog.ui
  opgrowdialog.cpp
  opgrowdialog.ui
  opkerneldialog.cpp
  opkerneldialog.ui
  opmediandialog.cpp
  opmediandialog.ui
  oprotatedialog.cpp
  oprotatedialog.ui
  opshiftdialog.cpp
  opshiftdialog.ui
  opshrinkdialog.cpp
  opshrinkdialog.ui
  opstackdialog.cpp
  opstackdialog.ui
  opunsharpmaskdialog.cpp
  opunsharpmaskdialog.ui
  synthesizebackgrounddialog.cpp
  synthesizebackgrounddialog.ui
  vancittertdeconvolutiondialog.cpp
  vancittertdeconvolutiondialog.ui
  test/gaussiantestimagedialog.cpp
  test/gaussiantestimagedialog.ui
  ${plugin_sources}
  ${plugin_mocs}
  ${private_header_list}
  ${plugin_headers}
  )

target_compile_definitions(fitsplugins PRIVATE ${FITS_DEF})

target_compile_options(fitsplugins PRIVATE -fPIC)

if (FFTW_FOUND)
  target_link_libraries(fitsplugins
    PUBLIC PkgConfig::FFTW
    )
endif()

target_link_libraries(fitsplugins
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

