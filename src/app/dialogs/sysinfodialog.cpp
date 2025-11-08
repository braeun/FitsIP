#include "sysinfodialog.h"
#include "ui_sysinfodialog.h"
#include "../appsettings.h"
#include <fitsip/core/fitsconfig.h>

SysInfoDialog::SysInfoDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::SysInfoDialog)
{
  ui->setupUi(this);
  QString options = "Build Options:\n";
#ifdef USE_FLOAT
  options += "  Value Type      = float\n";
#else
  options += "  ValueType = double\n";
#endif
#ifdef USE_16BITRAW
  options += "  Raw Image Input = 16 bits\n";
#else
#endif
  options += "Configuration:\n";
  options += "  Config Path     = " + Settings().getInternalDirectory() + "\n";

  ui->textBrowser->setText(options);
}

SysInfoDialog::~SysInfoDialog()
{
  delete ui;
}
