#include "pluginfilelistreturndialog.h"
#include "ui_pluginfilelistreturndialog.h"
#include <QPushButton>

PluginFilelistReturnDialog::PluginFilelistReturnDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::PluginFilelistReturnDialog),
  action(Ignore)
{
  ui->setupUi(this);

  QPushButton* append = ui->buttonBox->addButton("Append",QDialogButtonBox::AcceptRole);
  connect(append,&QPushButton::clicked,this,[this](){action=Append;});
  QPushButton* replace = ui->buttonBox->addButton("Replace",QDialogButtonBox::AcceptRole);
  replace->setDefault(true);
  connect(replace,&QPushButton::clicked,this,[this](){action=Replace;});
}

PluginFilelistReturnDialog::~PluginFilelistReturnDialog()
{
  delete ui;
}

void PluginFilelistReturnDialog::setListSize(size_t size)
{
  QString msg = "Plugin execution returned a list of " + QString::number(size) + " files. What do you want to do with it?";
  ui->msgLabel->setText(msg);
}


PluginFilelistReturnDialog::Action PluginFilelistReturnDialog::getAction() const
{
  return action;
}
