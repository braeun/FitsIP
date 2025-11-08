#ifndef ADDCAMERADIALOG_H
#define ADDCAMERADIALOG_H

#include <fitsip/core/db/camera.h>
#include <QDialog>

namespace Ui {
class AddCameraDialog;
}

class AddCameraDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AddCameraDialog(QWidget *parent = nullptr);
  ~AddCameraDialog();

  Camera getCamera() const;

private:
  Ui::AddCameraDialog *ui;
};

#endif // ADDCAMERADIALOG_H
