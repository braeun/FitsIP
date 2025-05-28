#include "opfftconvolutiondialog.h"
#include "ui_opfftconvolutiondialog.h"
#include <fitsip/core/widgets/psfwidget.h>

OpFFTConvolutionDialog::OpFFTConvolutionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpFFTConvolutionDialog)
{
  ui->setupUi(this);
  psfWidget = new PSFWidget(this);
  psfWidget->setObjectName(QString::fromUtf8("psfWidget"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(1);
  sizePolicy.setHeightForWidth(psfWidget->sizePolicy().hasHeightForWidth());
  psfWidget->setSizePolicy(sizePolicy);
  ui->verticalLayout->addWidget(psfWidget);
}

OpFFTConvolutionDialog::~OpFFTConvolutionDialog()
{
  delete ui;
}

void OpFFTConvolutionDialog::updatePSFList()
{
  psfWidget->updatePSFList();
}

QString OpFFTConvolutionDialog::getFunction() const
{
  return psfWidget->getFunction();
}

std::vector<ValueType> OpFFTConvolutionDialog::getParameters() const
{
  return psfWidget->getParameters();
}

