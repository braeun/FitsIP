#ifndef ADDTELESCOPEDIALOG_H
#define ADDTELESCOPEDIALOG_H

#include <fitsip/core/db/telescope.h>
#include <QDialog>

namespace Ui {
class AddTelescopeDialog;
}

class AddTelescopeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AddTelescopeDialog(QWidget *parent = nullptr);
  ~AddTelescopeDialog();

  Telescope getTelescope() const;

private:
  Ui::AddTelescopeDialog *ui;
};

#endif // ADDTELESCOPEDIALOG_H
