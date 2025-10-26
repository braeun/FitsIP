add_executable(fits
  annotationswidget.h annotationswidget.cpp
  appsettings.h appsettings.cpp
  consolewidget.h consolewidget.cpp consolewidget.ui
  cursors.qrc
  filelistwidget.h filelistwidget.cpp filelistwidget.ui
  filesystemview.h filesystemview.cpp filesystemview.ui
  histogramview.h histogramview.cpp histogramview.ui
  icons.qrc
  imagewidget.h imagewidget.cpp
  logbookwidget.h logbookwidget.cpp logbookwidget.ui
  logbookwidget.h logwidget.cpp logwidget.ui
  main.cpp
  mainwindow.h mainwindow.cpp mainwindow.ui
  palettefactory.h palettefactory.cpp
  pixellistwidget.h pixellistwidget.cpp pixellistwidget.ui
  profilertablemodel.h profilertablemodel.cpp
  profilerwidget.h profilerwidget.cpp profilerwidget.ui
  profileview.h profileview.cpp profileview.ui
  script.h script.cpp
  scriptinterface.h scriptinterface.cpp
  starlistwidget.h starlistwidget.cpp starlistwidget.ui
  staticplugins.cpp
  dialogs/aboutdialog.h dialogs/aboutdialog.cpp dialogs/aboutdialog.ui
  dialogs/addlogbookentrydialog.h dialogs/addlogbookentrydialog.cpp dialogs/addlogbookentrydialog.ui
  dialogs/configurationdialog.h dialogs/configurationdialog.cpp dialogs/configurationdialog.ui
  dialogs/editmetadatadialog.h dialogs/editmetadatadialog.cpp dialogs/editmetadatadialog.ui
  dialogs/logbookexportdialog.h dialogs/logbookexportdialog.h dialogs/logbookexportdialog.cpp dialogs/logbookexportdialog.ui
  dialogs/logbookfilterdialog.h dialogs/logbookfilterdialog.cpp dialogs/logbookfilterdialog.ui
  dialogs/logbookfilter1dialog.h dialogs/logbookfilter1dialog.cpp dialogs/logbookfilter1dialog.ui
  dialogs/logbookpropertiesdialog.h dialogs/logbookpropertiesdialog.cpp dialogs/logbookpropertiesdialog.ui
  dialogs/logentryeditdialog.h dialogs/logentryeditdialog.cpp dialogs/logentryeditdialog.ui
#  dialogs/psfmanagerdialog.cpp dialogs/psfmanagerdialog.ui
  )

target_compile_definitions(fits PRIVATE ${FITS_DEF})

target_include_directories(fits
PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src>
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
