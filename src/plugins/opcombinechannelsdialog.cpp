#include "opcombinechannelsdialog.h"
#include "ui_opcombinechannelsdialog.h"

OpCombineChannelsDialog::OpCombineChannelsDialog(QWidget *parent):
  QDialog(parent),
  ui(new Ui::OpCombineChannelsDialog)
{
  ui->setupUi(this);
}

OpCombineChannelsDialog::~OpCombineChannelsDialog()
{
  delete ui;
}

void OpCombineChannelsDialog::setCollection(const ImageCollection& collection)
{
  ui->redChannelBox->clear();
  ui->greenChannelBox->clear();
  ui->blueChannelBox->clear();
  for (const auto& img : collection.getFiles())
  {
    ui->redChannelBox->addItem(img->getImage()->getName(),img->getId());
    ui->greenChannelBox->addItem(img->getImage()->getName(),img->getId());
    ui->blueChannelBox->addItem(img->getImage()->getName(),img->getId());
  }
}

int OpCombineChannelsDialog::getRedImage() const
{
  return ui->redChannelBox->currentData().toInt();
}

int OpCombineChannelsDialog::getGreenImage() const
{
  return ui->greenChannelBox->currentData().toInt();
}

int OpCombineChannelsDialog::getBlueImage() const
{
  return ui->blueChannelBox->currentData().toInt();
}

