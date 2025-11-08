#include "addtelescopedialog.h"
#include "ui_addtelescopedialog.h"

AddTelescopeDialog::AddTelescopeDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::AddTelescopeDialog)
{
  ui->setupUi(this);
}

AddTelescopeDialog::~AddTelescopeDialog()
{
  delete ui;
}

Telescope AddTelescopeDialog::getTelescope() const
{
  QString name = ui->nameField->text();
  double f = ui->fField->text().toDouble();
  double d = ui->dField->text().toDouble();
  return Telescope(name,f,d);
}
