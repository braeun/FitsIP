set(private_header_list
  appsettings.h
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
  starlistwidget.h
  dialogs/aboutdialog.h
  dialogs/addlogbookentrydialog.h
  dialogs/configurationdialog.h
  dialogs/editmetadatadialog.h
  dialogs/logbookfilterdialog.h
  dialogs/logbookfilter1dialog.h
  dialogs/logbookpropertiesdialog.h
  dialogs/logentryeditdialog.h
)

add_executable(fits
  appsettings.cpp
  cursors.qrc
  filelistwidget.cpp
  filelistwidget.ui
  filesystemview.cpp
  filesystemview.ui
  histogramview.cpp
  histogramview.ui
  icons.qrc
  imagewidget.cpp
  logbookwidget.cpp
  logbookwidget.ui
  logwidget.cpp
  logwidget.ui
  main.cpp
  mainwindow.cpp
  mainwindow.ui
  palettefactory.cpp
  pixellistwidget.cpp
  pixellistwidget.ui
  profilertablemodel.cpp
  profilerwidget.cpp
  profilerwidget.ui
  profileview.cpp
  profileview.ui
  starlistwidget.cpp
  starlistwidget.ui
  staticplugins.cpp
  dialogs/aboutdialog.cpp
  dialogs/aboutdialog.ui
  dialogs/addlogbookentrydialog.cpp
  dialogs/addlogbookentrydialog.ui
  dialogs/configurationdialog.cpp
  dialogs/configurationdialog.ui
  dialogs/editmetadatadialog.cpp
  dialogs/editmetadatadialog.ui
  dialogs/logbookfilterdialog.cpp
  dialogs/logbookfilterdialog.ui
  dialogs/logbookfilter1dialog.cpp
  dialogs/logbookfilter1dialog.ui
  dialogs/logbookpropertiesdialog.cpp
  dialogs/logbookpropertiesdialog.ui
  dialogs/logentryeditdialog.cpp dialogs/logentryeditdialog.ui
  ${private_header_list}
  )

target_compile_definitions(fits PRIVATE ${FITS_DEF})

target_include_directories(fits
  PRIVATE ${QWT_INCLUDE_DIRS}
  )

target_link_libraries(fits
  PRIVATE fitsplugins
  PRIVATE fitsbase
  PRIVATE PkgConfig::QWT
  PRIVATE Qt5::Xml
  PRIVATE Qt5::Widgets
)

if (FFTW_FOUND)
#  target_include_directories(fits
#    PRIVATE ${FFTW_INCLUDE_DIRS}
#    )
  target_link_libraries(fits
    PRIVATE PkgConfig::FFTW
  )
endif()

if (LIBRAW_FOUND)
  target_link_libraries(fits
    PRIVATE PkgConfig::LIBRAW
    )
endif()

if (EXIV2_FOUND)
#  target_include_directories(fits
#    PRIVATE ${EXIV2_INCLUDE_DIRS}
#    )
  target_link_libraries(fits
    PRIVATE PkgConfig::EXIV2
  )
endif()

