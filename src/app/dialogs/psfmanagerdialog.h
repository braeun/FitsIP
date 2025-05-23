#ifndef PSFMANAGERDIALOG_H
#define PSFMANAGERDIALOG_H

#include <QDialog>

namespace Ui {
class PSFManagerDialog;
}

class PSFWidget;

class PSFManagerDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PSFManagerDialog(QWidget *parent = nullptr);
  ~PSFManagerDialog();

  void updatePSFList();

private:
  void preview();

  Ui::PSFManagerDialog *ui;
  PSFWidget* psfWidget;
};

#endif // PSFMANAGERDIALOG_H
