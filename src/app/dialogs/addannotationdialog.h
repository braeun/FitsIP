#ifndef ADDANNOTATIONDIALOG_H
#define ADDANNOTATIONDIALOG_H

#include <QDialog>

namespace Ui {
class AddAnnotationDialog;
}

class AddAnnotationDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AddAnnotationDialog(QWidget *parent = nullptr);
  ~AddAnnotationDialog();

  QString getText() const;

private:
  Ui::AddAnnotationDialog *ui;
};

#endif // ADDANNOTATIONDIALOG_H
