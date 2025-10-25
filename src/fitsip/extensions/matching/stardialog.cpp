#include "stardialog.h"
#include "ui_stardialog.h"

StarDialog::StarDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StarDialog)
{
  ui->setupUi(this);
}

StarDialog::~StarDialog()
{
  delete ui;
}

void StarDialog::setImageSkyValue(double v)
{
  ui->imageSkyLabel->setText(QString::number(v));
  ui->userSkyField->setText(QString::number(v));
}

int StarDialog::getBoxSize() const
{
  return ui->starBoxSpinner->value();
}

bool StarDialog::isUserSkyValue() const
{
  return ui->userSkyButton->isChecked();
}

ValueType StarDialog::getUserSkyValue() const
{
  return static_cast<ValueType>(ui->userSkyField->text().toFloat());
}


