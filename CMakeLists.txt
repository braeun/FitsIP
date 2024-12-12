cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

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
find_package(Qt5 COMPONENTS Widgets Xml REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# check for QWT charts
pkg_check_modules(QWT IMPORTED_TARGET Qt5Qwt6)

# check for CCfits library
pkg_check_modules(cfitsio IMPORTED_TARGET REQUIRED cfitsio)
pkg_check_modules(CCfits IMPORTED_TARGET REQUIRED CCfits)

# find astro libraries
# pkg_check_modules(LIBASTRO IMPORTED_TARGET libastro)

# check for libraw
pkg_check_modules(LIBRAW IMPORTED_TARGET libraw)

# find libraries for FFTW
pkg_check_modules(FFTW IMPORTED_TARGET fftw3)

#check for exif library
pkg_check_modules(EXIV2 IMPORTED_TARGET exiv2)

# find Lua+SOL2 for scripting
pkg_check_modules(SOL2 sol2)
if (SOL2_FOUND)
  find_package(Lua)
endif()
if (LUA_FOUND)
  set(USE_LUA ON)
endif()


#-----------------------------------------------------------------------------
# OPTIONS
#-----------------------------------------------------------------------------

option(USE_FLOAT "use float instead of double" ON)

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

if (USE_LUA)
  list(APPEND FITS_DEF -DHAVE_LUA)
endif()

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src)

#-----------------------------------------------------------------------------
# BUILD ALL COMPONENTS
#-----------------------------------------------------------------------------

add_subdirectory(src/fitsbase)
add_subdirectory(src/plugins)
add_subdirectory(src/app)


#-----------------------------------------------------------------------------
# INSTALL
#-----------------------------------------------------------------------------



