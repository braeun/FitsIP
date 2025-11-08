#ifndef CAMERADATABASEDIALOG_H
#define CAMERADATABASEDIALOG_H

#include <fitsip/core/db/cameratablemodel.h>
#include <QDialog>

namespace Ui {
class CameraDatabaseDialog;
}

class CameraDatabaseDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CameraDatabaseDialog(QWidget *parent = nullptr);
  ~CameraDatabaseDialog();

  static void showDialog();

private:
  void add();
  void remove();

  Ui::CameraDatabaseDialog *ui;
  CameraTableModel* model;

  static CameraDatabaseDialog* dlg;
};

#endif // CAMERADATABASEDIALOG_H
