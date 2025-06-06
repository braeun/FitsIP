
add_library(fitstestimages SHARED
  testplugincollection.h testplugincollection.cpp
  boxtestimage.h boxtestimage.cpp
  gaussiantestimage.h gaussiantestimage.cpp
  psftestimage.h psftestimage.cpp
  boxtestimagedialog.h boxtestimagedialog.cpp boxtestimagedialog.ui
  gaussiantestimagedialog.h gaussiantestimagedialog.cpp gaussiantestimagedialog.ui
  psftestimagedialog.h psftestimagedialog.cpp psftestimagedialog.ui
  plugin.json
  rulerblackonwhitetestimage.h rulerblackonwhitetestimage.cpp
  rulerwhiteonblacktestimage.h rulerwhiteonblacktestimage.cpp
  images.qrc
)

target_include_directories(fitstestimages
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitstestimages PRIVATE ${FITS_DEF})

target_compile_options(fitstestimages PRIVATE -fPIC)

target_link_libraries(fitstestimages
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitstestimages PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitstestimages
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitstestimages
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

