
add_library(fitspython SHARED
  python_bindings.cpp
)

add_library(fitsip::fitspython ALIAS fitspython)

set_target_properties(fitspython PROPERTIES PREFIX "")

target_link_libraries(fitspython
  PUBLIC fitsip::fitsbase
  PUBLIC pybind11::pybind11
#    PUBLIC Python::Module
)

install(TARGETS fitspython
  LIBRARY DESTINATION ${Python3_SITEARCH}
)
