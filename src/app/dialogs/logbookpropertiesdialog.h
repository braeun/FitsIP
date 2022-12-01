#ifndef LOGBOOKPROPERTIESDIALOG_H
#define LOGBOOKPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class LogbookPropertiesDialog;
}

class LogbookPropertiesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LogbookPropertiesDialog(QWidget *parent = nullptr);
  ~LogbookPropertiesDialog();

  void setTitleAndDescription(const QString& title, const QString& desc);

  QString getTitle() const;

  QString getDescription() const;

private:
  Ui::LogbookPropertiesDialog *ui;
};

#endif // LOGBOOKPROPERTIESDIALOG_H
