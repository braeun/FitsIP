set(private_header_list
  appsettings.h
  consolewidget.h
  filelistwidget.h
  filesystemview.h
  histogramview.h
  imagewidget.h
  logbookwidget.h
  logwidget.h
  mainwindow.h
  palettefactory.h
  pixellistwidget.h
  profilertablemodel.h
  profilerwidget.h
  profileview.h
  script.h
  scriptinterface.h
  starlistwidget.h
  dialogs/aboutdialog.h
  dialogs/addlogbookentrydialog.h
  dialogs/configurationdialog.h
  dialogs/editmetadatadialog.h
  dialogs/logbookfilterdialog.h
  dialogs/logbookfilter1dialog.h
  dialogs/logbookpropertiesdialog.h
  dialogs/logentryeditdialog.h
  dialogs/psfmanagerdialog.h
  dialogs/stardialog.h
)

add_executable(fits
  appsettings.cpp
  consolewidget.cpp consolewidget.ui
  cursors.qrc
  filelistwidget.cpp filelistwidget.ui
  filesystemview.cpp filesystemview.ui
  histogramview.cpp histogramview.ui
  icons.qrc
  imagewidget.cpp
  logbookwidget.cpp logbookwidget.ui
  logwidget.cpp logwidget.ui
  main.cpp
  mainwindow.cpp mainwindow.ui
  palettefactory.cpp
  pixellistwidget.cpp pixellistwidget.ui
  profilertablemodel.cpp
  profilerwidget.cpp profilerwidget.ui
  profileview.cpp profileview.ui
  script.cpp
  scriptinterface.cpp
  starlistwidget.cpp starlistwidget.ui
  staticplugins.cpp
  dialogs/aboutdialog.cpp dialogs/aboutdialog.ui
  dialogs/addlogbookentrydialog.cpp dialogs/addlogbookentrydialog.ui
  dialogs/configurationdialog.cpp dialogs/configurationdialog.ui
  dialogs/editmetadatadialog.cpp dialogs/editmetadatadialog.ui
  dialogs/logbookfilterdialog.cpp dialogs/logbookfilterdialog.ui
  dialogs/logbookfilter1dialog.cpp dialogs/logbookfilter1dialog.ui
  dialogs/logbookpropertiesdialog.cpp dialogs/logbookpropertiesdialog.ui
  dialogs/logentryeditdialog.cpp dialogs/logentryeditdialog.ui
  dialogs/psfmanagerdialog.cpp dialogs/psfmanagerdialog.ui
  dialogs/stardialog.cpp dialogs/stardialog.ui
  ${private_header_list}
  )

target_compile_definitions(fits PRIVATE ${FITS_DEF})

target_include_directories(fits
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:include>
)

#target_link_options(fits PRIVATE -rdynamic)

target_link_libraries(fits
#  PRIVATE fitsip::coreplugins
  PRIVATE fitsip::core
  PRIVATE PkgConfig::QWT
  PRIVATE Qt5::Xml
  PRIVATE Qt5::Widgets
)

if (FFTW_FOUND)
  target_link_libraries(fits
    PUBLIC PkgConfig::FFTW
  )
endif()

if (LIBRAW_FOUND)
  target_link_libraries(fits
    PRIVATE PkgConfig::LIBRAW
    )
endif()

if (EXIV2_FOUND)
  target_link_libraries(fits
    PRIVATE PkgConfig::EXIV2
  )
endif()

if (USE_PYTHON)
  # required by pybind11
  # maybe just set the class with __attribute__ ((visibility("hidden")))
  set_target_properties(fits PROPERTIES CXX_VISIBILITY_PRESET hidden)
  target_sources(fits
    PRIVATE pythonscript.h pythonscript.cpp
  )
  target_link_libraries(fits
    PRIVATE pybind11::embed
  )
endif()
