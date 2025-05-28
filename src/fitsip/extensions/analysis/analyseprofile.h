#ifndef ANALYSEPROFILE_H
#define ANALYSEPROFILE_H

#include <fitsip/core/fitstypes.h>
#include <fitsip/core/opplugin.h>
#include <QObject>
#include <vector>

class AnalyseProfileDialog;

class AnalyseProfile: public OpPlugin
{
  Q_OBJECT
  Q_INTERFACES(OpPlugin)
public:
  AnalyseProfile();
  ~AnalyseProfile();

  virtual QString getMenuEntry() const override;

  virtual ResultType execute(std::shared_ptr<FitsObject> image, const OpPluginData& data=OpPluginData()) override;

private:
  AnalyseProfileDialog* dlg;

};

#endif // ANALYSEPROFILE_H
