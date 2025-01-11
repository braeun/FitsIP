#include "analyseprofiledialog.h"
#include "ui_analyseprofiledialog.h"

AnalyseProfileDialog::AnalyseProfileDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AnalyseProfileDialog)
{
  ui->setupUi(this);
}

AnalyseProfileDialog::~AnalyseProfileDialog()
{
  delete ui;
}
