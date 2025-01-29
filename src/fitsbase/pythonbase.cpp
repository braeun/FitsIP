#include "pythonbase.h"
#include "fitsobject.h"

namespace pythonbase {

void bind(py::module_& m)
{
  py::class_<FitsObject, std::shared_ptr<FitsObject>>(m,"FitsObject","Fits Object")
      .def(py::init<std::shared_ptr<FitsImage>, QString>())
      ;
}

}

