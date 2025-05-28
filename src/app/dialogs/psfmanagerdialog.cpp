#include "psfmanagerdialog.h"
#include "ui_psfmanagerdialog.h"
#include <fitsip/core/imagestatistics.h>
#include <fitsip/core/psf/psffactory.h>
#include <fitsip/core/widgets/psfwidget.h>
#include <QAbstractButton>

PSFManagerDialog::PSFManagerDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PSFManagerDialog)
{
  ui->setupUi(this);
  psfWidget = new PSFWidget();
  ui->psfControlLayout->insertWidget(0,psfWidget,1);
  connect(ui->refreshButton,&QAbstractButton::clicked,this,[this](){updatePSFList();});
  connect(ui->previewButton,&QAbstractButton::clicked,this,[this](){preview();});
}

PSFManagerDialog::~PSFManagerDialog()
{
  delete ui;
}

void PSFManagerDialog::updatePSFList()
{
  psfWidget->updatePSFList();
}

void PSFManagerDialog::preview()
{
  int w = ui->previewWidthBox->value();
  int h = ui->previewHeightBox->value();
  const PSF* psf = PSFFactory::getInstance()->getPSF(psfWidget->getFunction());
  auto psfpar = psfWidget->getParameters();
  if (psf)
  {
    auto img = psf->createPSF(w,h,psfpar);
    ImageStatistics stat(*img);
    auto qimg = img->toQImage(stat.getGlobalStatistics().minValue,stat.getGlobalStatistics().maxValue,FitsImage::LOG);
    ui->previewLabel->setPixmap(QPixmap::fromImage(qimg));
  }
}
