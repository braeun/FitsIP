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
  analyseprofile.h
  findstars.h
  measurecrosscorrelation.h
  measurematch.h
  measurerank.h
  measurestatistics.h
  opadd.h
  opalign.h
  opaverage.h
  opcalibration.h
  opcombinechannels.h
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
  opresize.h
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
  test/psftestimage.h
  )

set(plugin_sources
  analyseprofile.cpp
  findstars.cpp
  measurecrosscorrelation.cpp
  measurematch.cpp
  measurerank.cpp
  measurestatistics.cpp
  opadd.cpp
  opalign.cpp
  opaverage.cpp
  opcalibration.cpp
  opcombinechannels.cpp
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
  opresize.cpp
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
  test/psftestimage.cpp
  )

if (FFTW_FOUND)
  list(APPEND plugin_headers
    lucyrichardsondeconvolution.h
    opfft.h
    opinvfft.h
    vancittertdeconvolution.h
    )
  list(APPEND plugin_sources
    lucyrichardsondeconvolution.cpp
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
  OPTIONS -I${PROJECT_SOURCE_DIR}/src
  )

set(private_header_list
  analyseprofiledialog.h
  findstarsdialog.h
  lucyrichardsondeconvolutiondialog.h
  measurematchdialog.h
  measurerankresultdialog.h
  opaligndialog.h
  opcalibrationdialog.h
  opcombinechannelsdialog.h
  opcropdialog.h
  opddpdialog.h
  opgaussblurdialog.h
  opgrowdialog.h
  opkerneldialog.h
  opmediandialog.h
  opresizedialog.h
  oprotatedialog.h
  opshiftdialog.h
  opshrinkdialog.h
  opstackdialog.h
  opunsharpmaskdialog.h
  synthesizebackgrounddialog.h
  vancittertdeconvolutiondialog.h
  test/gaussiantestimagedialog.h
  test/psftestimagedialog.h
)

add_library(fitsplugins
  analyseprofiledialog.cpp analyseprofiledialog.ui
  findstarsdialog.cpp findstarsdialog.ui
  lucyrichardsondeconvolutiondialog.cpp lucyrichardsondeconvolutiondialog.ui
  measurematchdialog.cpp measurematchdialog.ui
  measurerankresultdialog.cpp measurerankresultdialog.ui
  opaligndialog.cpp opaligndialog.ui
  opcalibrationdialog.cpp opcalibrationdialog.ui
  opcombinechannelsdialog.cpp opcombinechannelsdialog.ui
  opcropdialog.cpp opcropdialog.ui
  opddpdialog.cpp opddpdialog.ui
  opgaussblurdialog.cpp opgaussblurdialog.ui
  opgrowdialog.cpp opgrowdialog.ui
  opkerneldialog.cpp opkerneldialog.ui
  opmediandialog.cpp opmediandialog.ui
  opresizedialog.cpp opresizedialog.ui
  oprotatedialog.cpp oprotatedialog.ui
  opshiftdialog.cpp opshiftdialog.ui
  opshrinkdialog.cpp opshrinkdialog.ui
  opstackdialog.cpp opstackdialog.ui
  opunsharpmaskdialog.cpp opunsharpmaskdialog.ui
  pluginresources.qrc
  synthesizebackgrounddialog.cpp synthesizebackgrounddialog.ui
  vancittertdeconvolutiondialog.cpp vancittertdeconvolutiondialog.ui
  test/gaussiantestimagedialog.cpp test/gaussiantestimagedialog.ui
  test/psftestimagedialog.cpp test/psftestimagedialog.ui
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
  PUBLIC fitsbase
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

