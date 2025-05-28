#ifndef PLUGINFILELISTRETURNDIALOG_H
#define PLUGINFILELISTRETURNDIALOG_H

#include <QDialog>

namespace Ui {
class PluginFilelistReturnDialog;
}

class PluginFilelistReturnDialog : public QDialog
{
  Q_OBJECT
public:

  enum Action { Ignore, Append, Replace };

  explicit PluginFilelistReturnDialog(QWidget *parent = nullptr);
  ~PluginFilelistReturnDialog();

  void setListSize(size_t size);

  Action getAction() const;

private:
  Ui::PluginFilelistReturnDialog *ui;
  Action action;
};

#endif // PLUGINFILELISTRETURNDIALOG_H
