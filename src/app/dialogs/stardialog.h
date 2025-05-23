#ifndef STARDIALOG_H
#define STARDIALOG_H

#include <fitsbase/fitstypes.h>
#include <QDialog>

namespace Ui {
class StarDialog;
}

class StarDialog : public QDialog
{
  Q_OBJECT

public:
  explicit StarDialog(QWidget *parent = nullptr);
  ~StarDialog();

  int getBoxSize() const;

  bool isUserSkyValue() const;

  ValueType getUserSkyValue() const;

private:
  Ui::StarDialog *ui;
};

#endif // STARDIALOG_H
