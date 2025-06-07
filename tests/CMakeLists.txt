
add_executable(gausfit_test
  gaussfit.cpp
)
target_include_directories(gausfit_test
PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gausfit_test
  PRIVATE fitsip::core
)
if (EXIV2_FOUND)
  target_link_libraries(gausfit_test
    PRIVATE PkgConfig::EXIV2
  )
endif()

