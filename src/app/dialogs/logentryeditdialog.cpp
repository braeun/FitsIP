#include "logentryeditdialog.h"
#include "ui_logentryeditdialog.h"

LogEntryEditDialog::LogEntryEditDialog(const LogbookEntry& entry, const QStringList& projects, const QStringList& steps, QWidget *parent):QDialog(parent),
  ui(new Ui::LogEntryEditDialog),
  entry(entry)
{
  ui->setupUi(this);
  ui->timestampLabel->setText(entry.getTimestamp().toString(Qt::ISODateWithMs));
  ui->projectBox->addItems(projects);
  ui->projectBox->setCurrentText(entry.getProject());
  ui->stepBox->addItems(steps);
  ui->stepBox->setCurrentText(entry.getStep());
  ui->objectField->setText(entry.getImage());
  ui->typeBox->setCurrentIndex(entry.getType());
  ui->textField->setPlainText(entry.getText());
}

LogEntryEditDialog::~LogEntryEditDialog()
{
  delete ui;
}

LogbookEntry LogEntryEditDialog::getEntry()
{
  entry.setProject(ui->projectBox->currentText());
  entry.setStep(ui->stepBox->currentText());
  entry.setImage(ui->objectField->text());
  entry.setType(static_cast<LogbookEntry::Type>(ui->typeBox->currentIndex()));
  entry.setText(ui->textField->toPlainText());
  return entry;
}
