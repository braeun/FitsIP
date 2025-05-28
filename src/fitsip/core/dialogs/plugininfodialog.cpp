#include "plugininfodialog.h"
#include "ui_plugininfodialog.h"
#include "../pluginfactory.h"

PluginInfoDialog::PluginInfoDialog(PluginFactory* factory, QWidget *parent):QDialog(parent),
    ui(new Ui::PluginInfoDialog)
{
  ui->setupUi(this);
  QStringList pathlist = factory->getSearchPath();
  ui->searchPathWidget->addItems(pathlist);
  auto libs = factory->getLibraryInfo();
  ui->libraryTableWidget->setRowCount(libs.size());
  for (size_t row=0;row<libs.size();++row)
  {
    ui->libraryTableWidget->setItem(row,0,new QTableWidgetItem(libs[row].name));
    ui->libraryTableWidget->setItem(row,1,new QTableWidgetItem(libs[row].filename));
  }
}

PluginInfoDialog::~PluginInfoDialog()
{
  delete ui;
}
