
add_library(fitsmatching SHARED
  findstars.h findstars.cpp
  findstarsdialog.h findstarsdialog.cpp findstarsdialog.ui
  matchingplugincollection.h matchingplugincollection.cpp
  measurematch.h measurematch.cpp
  measurematchdialog.h measurematchdialog.cpp measurematchdialog.ui
  plugin.json
)

add_library(fitsip::matching ALIAS fitsmatching)

target_include_directories(fitsmatching
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsmatching PRIVATE ${FITS_DEF})

target_compile_options(fitsmatching PRIVATE -fPIC)

target_link_libraries(fitsmatching
  PUBLIC fitsip::coreplugins
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitsmatching PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsmatching
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitsmatching
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
