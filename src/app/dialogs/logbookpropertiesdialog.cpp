#include "logbookpropertiesdialog.h"
#include "ui_logbookpropertiesdialog.h"

LogbookPropertiesDialog::LogbookPropertiesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LogbookPropertiesDialog)
{
  ui->setupUi(this);
}

LogbookPropertiesDialog::~LogbookPropertiesDialog()
{
  delete ui;
}

void LogbookPropertiesDialog::setTitleAndDescription(const QString &title, const QString &desc)
{
  ui->titleField->setText(title);
  ui->descriptionField->setPlainText(desc);
}

QString LogbookPropertiesDialog::getTitle() const
{
  return ui->titleField->text();
}

QString LogbookPropertiesDialog::getDescription() const
{
  return ui->descriptionField->toPlainText();
}

