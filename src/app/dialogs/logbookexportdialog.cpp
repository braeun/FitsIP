#include "logbookexportdialog.h"
#include "ui_logbookexportdialog.h"
#include "../appsettings.h"
#include <fitsip/core/logbook/logbook.h>
#include <QFileDialog>
#include <QFileInfo>

LogbookExportDialog::LogbookExportDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LogbookExportDialog)
{
  ui->setupUi(this);
  templates = Logbook::getTemplates();
  for (QString t : templates)
  {
    QFileInfo info(t);
    ui->templateBox->addItem(info.baseName());
  }
  connect(ui->browseButton,&QPushButton::clicked,this,[this](){browse();});
}

LogbookExportDialog::~LogbookExportDialog()
{
  delete ui;
}

QString LogbookExportDialog::getFilename() const
{
  return ui->filenameField->text();
}

QString LogbookExportDialog::getTemplate() const
{
  int index = ui->templateBox->currentIndex();
  if (index < 0) return "";
  QString s = "";
  QFile file(templates[index]);
  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    s = file.readAll();
  }
  file.close();
  return s;
}



void LogbookExportDialog::browse()
{
  AppSettings settings;
  QString filter = "Plain Text (*.txt)";
  int index = ui->templateBox->currentIndex();
  if (index >= 0)
  {
    QFileInfo info(templates[index]);
    filter = info.baseName() + " (*." + info.suffix() + ")";
  }
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,filter);
  if (!fn.isNull())
  {
    ui->filenameField->setText(fn);
  }

}
