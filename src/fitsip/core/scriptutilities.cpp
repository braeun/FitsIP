#include "scriptutilities.h"

namespace ScriptUtilities
{

std::vector<std::string> convertStringList(const std::vector<QString>& list)
{
  std::vector<std::string> v;
  for (const QString& s : list)
  {
    v.push_back(s.toStdString());
  }
  return v;
}


}
