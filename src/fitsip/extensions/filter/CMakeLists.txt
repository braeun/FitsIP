
add_library(fitsfilter SHARED
  filterplugincollection.h filterplugincollection.cpp
  opddp.h opddp.cpp
  opddpdialog.h opddpdialog.cpp opddpdialog.ui
  opgaussblur.h opgaussblur.cpp
  opgaussblurdialog.h opgaussblurdialog.cpp opgaussblurdialog.ui
  opkernel.h opkernel.cpp
  opkerneldialog.h opkerneldialog.cpp opkerneldialog.ui
  opmedian.h opmedian.cpp
  opmediandialog.h opmediandialog.cpp opmediandialog.ui
  opsobel.h opsobel.cpp
  opunsharpmask.h opunsharpmask.cpp
  opunsharpmaskdialog.h opunsharpmaskdialog.cpp opunsharpmaskdialog.ui
  plugin.json
  filterresources.qrc
)

add_library(fitsip::filter ALIAS fitsfilter)

target_include_directories(fitsfilter
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(fitsfilter PRIVATE ${FITS_DEF})

target_compile_options(fitsfilter PRIVATE -fPIC)

target_link_libraries(fitsfilter
  PUBLIC fitsip::coreplugins
  PUBLIC fitsip::core
  PUBLIC Qt5::Widgets
  PUBLIC Qt5::Core
  )

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
#  set_target_properties(fitsfilter PROPERTIES CXX_VISIBILITY_PRESET hidden)

  target_link_libraries(fitsfilter
    PRIVATE pybind11::embed
  )
endif()

install(TARGETS fitsfilter
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS fitsfilter
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/fits
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/fits
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

