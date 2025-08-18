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
  for (const QString& t : templates)
  {
    QFileInfo info(t);
    ui->templateBox->addItem(info.baseName());
  }
  connect(ui->browseButton,&QPushButton::clicked,this,[this](){browse();});
  connect(ui->templateBrowseButton,&QPushButton::clicked,this,[this](){browseTemplate();});
  connect(ui->templateBox,&QComboBox::currentTextChanged,this,[this](){ui->predefinedButton->setChecked(true);});
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
  QString s = "";
  QString filename = "";
  if (ui->predefinedButton->isChecked())
  {
    int index = ui->templateBox->currentIndex();
    if (index >= 0) filename = templates[index];
  }
  else
  {
    filename = ui->templateFilenameField->text();
  }
  if (!filename.isEmpty())
  {
    QFile file(filename);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
      s = file.readAll();
    }
    file.close();
  }
  return s;
}



void LogbookExportDialog::browse()
{
  AppSettings settings;
  QString filter;
//  int index = ui->templateBox->currentIndex();
//  if (index >= 0)
//  {
//    QFileInfo info(templates[index]);
//    filter = info.baseName() + " (*." + info.suffix() + ")";
//  }
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,filter);
  if (!fn.isNull())
  {
    ui->filenameField->setText(fn);
  }
}


void LogbookExportDialog::browseTemplate()
{
  AppSettings settings;
  QString filter;
  QString fn = settings.getSaveFilename(this,AppSettings::PATH_LOG,filter);
  if (!fn.isNull())
  {
    ui->templateFilenameField->setText(fn);
    ui->fromFileButton->setChecked(true);
  }
}


