
add_executable(fits
  appsettings.h appsettings.cpp
  cursors.qrc
  icons.qrc
  main.cpp
  mainwindow.h mainwindow.cpp mainwindow.ui
  palettefactory.h palettefactory.cpp
  script.h script.cpp
  scriptinterface.h scriptinterface.cpp
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
  widgets/annotationswidget.h widgets/annotationswidget.cpp
  widgets/consolewidget.h widgets/consolewidget.cpp widgets/consolewidget.ui
  widgets/filelistwidget.h widgets/filelistwidget.cpp widgets/filelistwidget.ui
  widgets/filesystemview.h widgets/filesystemview.cpp widgets/filesystemview.ui
  widgets/histogramview.h widgets/histogramview.cpp widgets/histogramview.ui
  widgets/imagewidget.h widgets/imagewidget.cpp
  widgets/metadatatablewidget.h widgets/metadatatablewidget.cpp widgets/metadatatablewidget.ui
  widgets/logbookwidget.h widgets/logbookwidget.cpp widgets/logbookwidget.ui
  widgets/logbookwidget.h widgets/logwidget.cpp widgets/logwidget.ui
  widgets/pixellistwidget.h widgets/pixellistwidget.cpp widgets/pixellistwidget.ui
  widgets/profilertablemodel.h widgets/profilertablemodel.cpp
  widgets/profilerwidget.h widgets/profilerwidget.cpp widgets/profilerwidget.ui
  widgets/profileview.h widgets/profileview.cpp widgets/profileview.ui
  widgets/starlistwidget.h widgets/starlistwidget.cpp widgets/starlistwidget.ui
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
