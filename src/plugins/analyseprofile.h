#ifndef ANALYSEPROFILE_H
#define ANALYSEPROFILE_H

#include <fitsbase/fitstypes.h>
#include <fitsbase/opplugin.h>
#include <QObject>
#include <vector>

#define QT_STATICPLUGIN
#include <QtPlugin>

class AnalyseProfileDialog;

class AnalyseProfile: public OpPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID OpPlugin_iid)
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
