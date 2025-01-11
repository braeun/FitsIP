#ifndef ANALYSEPROFILEDIALOG_H
#define ANALYSEPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class AnalyseProfileDialog;
}

class AnalyseProfileDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AnalyseProfileDialog(QWidget *parent = nullptr);
  ~AnalyseProfileDialog();

private:
  Ui::AnalyseProfileDialog *ui;
};

#endif // ANALYSEPROFILEDIALOG_H
