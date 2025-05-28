
add_library(fitsstacking SHARED
  opalign.h opalign.cpp
  opaligndialog.h opaligndialog.cpp opaligndialog.ui
  opcalibration.h opcalibration.cpp
  opcalibrationdialog.h opcalibrationdialog.cpp opcalibrationdialog.ui
  opstack.h opstack.cpp
  opstackdialog.h opstackdialog.cpp opstackdialog.ui
  stackplugincollection.h stackplugincollection.cpp
  plugin.json
)

target_include_directories(fitsstacking
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsstacking PRIVATE ${FITS_DEF})

target_compile_options(fitsstacking PRIVATE -fPIC)

target_link_libraries(fitsstacking
#  PUBLIC fitsip::matching
#  PUBLIC fitsip::coreplugins
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitsstacking PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsstacking
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitsstacking
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


