#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl_bind.h>
#include <pybind11/embed.h>
#include <fitsbase/fitsimage.h>

#define PYBIND11_DETAILED_ERROR_MESSAGES

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(fitspython,m) {

}
