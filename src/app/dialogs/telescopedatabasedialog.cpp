#include "telescopedatabasedialog.h"
#include "ui_telescopedatabasedialog.h"
#include "addtelescopedialog.h"
#include <QMessageBox>

TelescopeDatabaseDialog* TelescopeDatabaseDialog::dlg = nullptr;

TelescopeDatabaseDialog::TelescopeDatabaseDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::TelescopeDatabaseDialog)
{
  ui->setupUi(this);
  ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  model = new TelescopeTableModel();
  ui->tableView->setModel(model);

  connect(ui->refreshButton,&QPushButton::clicked,this,[this](){model->refresh();});
  connect(ui->removeButton,&QPushButton::clicked,this,[this](){remove();});
  connect(ui->addButton,&QPushButton::clicked,this,[this](){add();});
}

TelescopeDatabaseDialog::~TelescopeDatabaseDialog()
{
  delete ui;
  delete model;
}

void TelescopeDatabaseDialog::showDialog()
{
  if (!dlg)
  {
    dlg = new TelescopeDatabaseDialog();
  }
  dlg->model->refresh();
  dlg->show();
}

void TelescopeDatabaseDialog::add()
{
  AddTelescopeDialog dlg(this);
  if (dlg.exec())
  {
    Telescope telescope = dlg.getTelescope();
    model->addTelescope(telescope);
  }
}

void TelescopeDatabaseDialog::remove()
{
  QModelIndexList list = ui->tableView->selectionModel()->selectedIndexes();
  if (QMessageBox::question(this,"Remove Telescope","Really delete the selected telescopes from the database?") == QMessageBox::Yes)
  {
    model->remove(list);
  }
}

