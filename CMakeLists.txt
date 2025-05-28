cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(FitsIP VERSION 0.5.0 LANGUAGES CXX)

# Forbid in-tree building
if(CMAKE_SOURCE_DIR MATCHES CMAKE_BINARY_DIR)
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif()

include(CMakeDependentOption)

#-----------------------------------------------------------------------------
# BUILD SYSTEM
#-----------------------------------------------------------------------------

set(CMAKE_VERBOSE_MAKEFILE OFF)

#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)

include(CheckTypeSize)
check_type_size("int" SIZEOF_INT LANGUAGE CXX)
if (${SIZEOF_INT} LESS "4")
  message(FATAL_ERROR "int must be at least 32bit")
endif()

include(GNUInstallDirs)

# keep rpath to actual Qt framework in executable
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

#-----------------------------------------------------------------------------
# DEPENDENCIES
#-----------------------------------------------------------------------------

# Specify search path for CMake modules to be loaded by include() and find_package()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# CMake config files
include(CMakePackageConfigHelpers)

# load the package to obtain information from pkgconfig
# ensure that packages etc. in /usr/local are found
# do NOT use CMAKE_PREFIX_PATH as it is used by qtcreator to select the Qt version
set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig:/usr/local/lib64/pkgconfig")
find_package(PkgConfig REQUIRED)

# check for Qt5
find_package(Qt5 COMPONENTS Widgets Xml Sql REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# check for QWT charts
pkg_check_modules(QWT IMPORTED_TARGET Qt5Qwt6)

# check for CCfits library
pkg_check_modules(cfitsio IMPORTED_TARGET REQUIRED cfitsio)
pkg_check_modules(CCfits IMPORTED_TARGET REQUIRED CCfits)

pkg_check_modules(EIGEN3 IMPORTED_TARGET REQUIRED eigen3)

# find astro libraries
# pkg_check_modules(LIBASTRO IMPORTED_TARGET libastro)

# check for libraw
pkg_check_modules(LIBRAW IMPORTED_TARGET libraw)

# find libraries for FFTW
pkg_check_modules(FFTW IMPORTED_TARGET fftw3)

#check for exif library
pkg_check_modules(EXIV2 IMPORTED_TARGET exiv2)

find_package(Python3 COMPONENTS Interpreter Development)
if (Python3_FOUND)
#  set(PYBIND11_NOPYTHON ON)
#set(PYBIND11_FINDPYTHON ON)
  find_package(pybind11 CONFIG)
  if (NOT pybind11_FOUND)
    message("pybind11 not found - not building python bindings")
  else()
    message("site packages ${Python3_SITEARCH}")
    set(USE_PYTHON ON)
  endif()
else()
  message("python3 not found - not building python bindings")
endif()

# find the json package by nlohmann for I/O
find_package(nlohmann_json)
if (nlohmann_json_FOUND)
  # find the inja template engine
  find_package(inja)
endif()




#-----------------------------------------------------------------------------
# OPTIONS
#-----------------------------------------------------------------------------

option(USE_FLOAT "use float instead of double" ON)
option(USE_16BITRAW "import raw image files as 16 bit" ON)

#-----------------------------------------------------------------------------
# COMPILE DEFINITIONS
#-----------------------------------------------------------------------------

set(FITS_DEF)
list(APPEND FITS_DEF -DFITS_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
list(APPEND FITS_DEF -DFITS_VERSION_MINOR=${PROJECT_VERSION_MINOR})
list(APPEND FITS_DEF -DFITS_VERSION_PATCH=${PROJECT_VERSION_PATCH})

if (LIBRAW_FOUND)
  list(APPEND FITS_DEF -DHAVE_LIBRAW)
endif()

if (FFTW_FOUND)
  list(APPEND FITS_DEF -DHAVE_FFTW)
endif()

if (EXIV2_FOUND)
  list(APPEND FITS_DEF -DHAVE_EXIV2)
endif()

if (USE_FLOAT)
  list(APPEND FITS_DEF -DUSE_FLOAT)
endif()

if (USE_16BITRAW)
  list(APPEND FITS_DEF -DUSE_16BITRAW)
endif()

if (USE_PYTHON)
  list(APPEND FITS_DEF -DUSE_PYTHON)
endif()

if (nlohmann_json_FOUND)
  list(APPEND FITS_DEF -DHAVE_JSON)
endif()

if (inja_FOUND)
  list(APPEND FITS_DEF -DHAVE_INJA)
endif()

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src)

#-----------------------------------------------------------------------------
# BUILD ALL COMPONENTS
#-----------------------------------------------------------------------------

add_subdirectory(src/fitsip/core)
add_subdirectory(src/fitsip/coreplugins)
add_subdirectory(src/fitsip/extensions)

if (USE_PYTHON)
endif()

add_subdirectory(src/app)


#-----------------------------------------------------------------------------
# INSTALL
#-----------------------------------------------------------------------------



