#include "addannotationdialog.h"
#include "ui_addannotationdialog.h"

AddAnnotationDialog::AddAnnotationDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::AddAnnotationDialog)
{
  ui->setupUi(this);
}

AddAnnotationDialog::~AddAnnotationDialog()
{
  delete ui;
}

QString AddAnnotationDialog::getText() const
{
  return ui->textField->text();
}

