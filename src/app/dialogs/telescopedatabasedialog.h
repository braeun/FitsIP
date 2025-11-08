#ifndef TELESCOPEDATABASEDIALOG_H
#define TELESCOPEDATABASEDIALOG_H

#include <fitsip/core/db/telescopetablemodel.h>
#include <QDialog>

namespace Ui {
class TelescopeDatabaseDialog;
}

class TelescopeDatabaseDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TelescopeDatabaseDialog(QWidget *parent = nullptr);
  ~TelescopeDatabaseDialog();

  static void showDialog();

private:
  void add();
  void remove();

  Ui::TelescopeDatabaseDialog *ui;
  TelescopeTableModel* model;

  static TelescopeDatabaseDialog* dlg;
};

#endif // TELESCOPEDATABASEDIALOG_H
