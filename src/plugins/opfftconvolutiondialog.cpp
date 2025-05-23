#include "opfftconvolutiondialog.h"
#include "ui_opfftconvolutiondialog.h"

OpFFTConvolutionDialog::OpFFTConvolutionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OpFFTConvolutionDialog)
{
  ui->setupUi(this);
}

OpFFTConvolutionDialog::~OpFFTConvolutionDialog()
{
  delete ui;
}

void OpFFTConvolutionDialog::updatePSFList()
{
  ui->psfWidget->updatePSFList();
}

QString OpFFTConvolutionDialog::getFunction() const
{
  return ui->psfWidget->getFunction();
}

std::vector<ValueType> OpFFTConvolutionDialog::getParameters() const
{
  return ui->psfWidget->getParameters();
}

