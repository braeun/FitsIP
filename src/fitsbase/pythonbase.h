#ifndef PYTHONBASE_H
#define PYTHONBASE_H

#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace pythonbase {

extern void bind(py::module_& m);

}

#endif // PYTHONBASE_H
