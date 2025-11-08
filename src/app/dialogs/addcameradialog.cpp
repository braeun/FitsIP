#include "addcameradialog.h"
#include "ui_addcameradialog.h"

AddCameraDialog::AddCameraDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AddCameraDialog)
{
  ui->setupUi(this);
}

AddCameraDialog::~AddCameraDialog()
{
  delete ui;
}

Camera AddCameraDialog::getCamera() const
{
  QString name = ui->nameField->text();
  double w = ui->pixelWidthField->text().toDouble();
  double h = ui->pixelHeightField->text().toDouble();
  return Camera(name,w,h);
}
