#ifndef OPFFTCONVOLUTIONDIALOG_H
#define OPFFTCONVOLUTIONDIALOG_H

#include <fitsbase/fitstypes.h>
#include <QDialog>

namespace Ui {
class OpFFTConvolutionDialog;
}

class OpFFTConvolutionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpFFTConvolutionDialog(QWidget *parent = nullptr);
  ~OpFFTConvolutionDialog();

  void updatePSFList();

  QString getFunction() const;

  std::vector<ValueType> getParameters() const;

private:
  Ui::OpFFTConvolutionDialog *ui;
};

#endif // OPFFTCONVOLUTIONDIALOG_H
