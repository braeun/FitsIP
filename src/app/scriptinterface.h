#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#include <fitsbase/fitsobject.h>
#include <memory>


#ifdef USE_PYTHON
#undef SLOT
#undef slot
#undef slots
#include <pybind11/pybind11.h>
namespace py = pybind11;
#endif


class ScriptInterface: public QObject
{
  Q_OBJECT
public:

  std::shared_ptr<FitsObject> load(const std::string& filename);

#ifdef USE_PYTHON
  static void bind(py::module_& m);
#endif

  static ScriptInterface* getInterface();

private:
  ScriptInterface();

  static std::unique_ptr<ScriptInterface> interface;
};

#endif // SCRIPTINTERFACE_H
