#include "cameradatabasedialog.h"
#include "ui_cameradatabasedialog.h"
#include "addcameradialog.h"
#include <QMessageBox>

CameraDatabaseDialog* CameraDatabaseDialog::dlg = nullptr;

CameraDatabaseDialog::CameraDatabaseDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::CameraDatabaseDialog)
{
  ui->setupUi(this);
  ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  model = new CameraTableModel();
  ui->tableView->setModel(model);

  connect(ui->refreshButton,&QPushButton::clicked,this,[this](){model->refresh();});
  connect(ui->removeButton,&QPushButton::clicked,this,[this](){remove();});
  connect(ui->addButton,&QPushButton::clicked,this,[this](){add();});
}

CameraDatabaseDialog::~CameraDatabaseDialog()
{
  delete ui;
  delete model;
}

void CameraDatabaseDialog::showDialog()
{
  if (!dlg)
  {
    dlg = new CameraDatabaseDialog();
  }
  dlg->model->refresh();
  dlg->show();
}

void CameraDatabaseDialog::add()
{
  AddCameraDialog dlg(this);
  if (dlg.exec())
  {
    Camera camera = dlg.getCamera();
    model->addCamera(camera);
  }
}

void CameraDatabaseDialog::remove()
{
  QModelIndexList list = ui->tableView->selectionModel()->selectedIndexes();
  if (QMessageBox::question(this,"Remove Cameras","Really delete the selected cameras from the database?") == QMessageBox::Yes)
  {
    model->remove(list);
  }
}

