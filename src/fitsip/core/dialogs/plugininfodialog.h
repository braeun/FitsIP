#ifndef PLUGININFODIALOG_H
#define PLUGININFODIALOG_H

#include <QDialog>

namespace Ui {
class PluginInfoDialog;
}

class PluginFactory;

class PluginInfoDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PluginInfoDialog(PluginFactory* factory, QWidget *parent = nullptr);
  ~PluginInfoDialog();

private:
  Ui::PluginInfoDialog *ui;
};

#endif // PLUGININFODIALOG_H
