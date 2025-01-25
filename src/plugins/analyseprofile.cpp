#include "analyseprofile.h"
#include "analyseprofiledialog.h"

AnalyseProfile::AnalyseProfile():OpPlugin(true),
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

OpPlugin::ResultType AnalyseProfile::execute(std::shared_ptr<FitsObject> image, QRect rect, const PreviewOptions& opt)
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
