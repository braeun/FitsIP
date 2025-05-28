#include "analyseprofile.h"
#include "analyseprofiledialog.h"

AnalyseProfile::AnalyseProfile():OpPlugin(),
  dlg(nullptr)
{
}

AnalyseProfile::~AnalyseProfile()
{
}

QString AnalyseProfile::getMenuEntry() const
{
  return "Analyse/Analyse Profiles...";
}

OpPlugin::ResultType AnalyseProfile::execute(std::shared_ptr<FitsObject> image, const OpPluginData& data)
{
  if (image->getXProfile().empty() || image->getYProfile().empty())
  {
    setError("No profiles selected");
    return ERROR;
  }
  if (!dlg)
  {
    dlg = new AnalyseProfileDialog();
  }
  dlg->setImage(image.get());
  dlg->exec();
  return OK;
}
