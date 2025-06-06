
add_library(fitscoreplugins SHARED
  opadd.cpp
  opaverage.cpp
  opcombinechannels.cpp
  opcombinechannelsdialog.h opcombinechannelsdialog.cpp opcombinechannelsdialog.ui
  opcrop.cpp
  opcropdialog.h opcropdialog.cpp opcropdialog.ui
  opcut.cpp
  opdiv.cpp
  opflipx.cpp
  opflipy.cpp
  oplog.cpp
  opmul.cpp
  opresize.cpp
  opresizedialog.h opresizedialog.cpp opresizedialog.ui
  oprotate.cpp
  oprotatedialog.h oprotatedialog.cpp oprotatedialog.ui
  opscale.cpp
  opshift.cpp
  opshiftdialog.h opshiftdialog.cpp opshiftdialog.ui
  opsplitchannels.cpp
  opsqrt.cpp
  opsub.cpp
  optogray.cpp
  pluginresources.qrc
  coreplugincollection.h coreplugincollection.cpp
  plugin.json
)
add_library(fitsip::coreplugins ALIAS fitscoreplugins)

target_sources(fitscoreplugins PUBLIC
  FILE_SET HEADERS TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../.." FILES
  opadd.h
  opaverage.h
  opcombinechannels.h
  opcrop.h
  opcut.h
  opdiv.h
  opflipx.h
  opflipy.h
  oplog.h
  opmul.h
  opresize.h
  oprotate.h
  opscale.h
  opshift.h
  opsplitchannels.h
  opsqrt.h
  opsub.h
  optogray.h
)

target_include_directories(fitscoreplugins
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitscoreplugins PRIVATE ${FITS_DEF})

target_compile_options(fitscoreplugins PRIVATE -fPIC)

target_link_libraries(fitscoreplugins
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitscoreplugins PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitscoreplugins
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitscoreplugins
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

