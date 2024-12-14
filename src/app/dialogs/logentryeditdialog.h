#ifndef LOGENTRYEDITDIALOG_H
#define LOGENTRYEDITDIALOG_H

#include <fitsbase/logbook/logbookentry.h>
#include <QDialog>
#include <QStringList>

namespace Ui {
class LogEntryEditDialog;
}

class LogEntryEditDialog: public QDialog
{
  Q_OBJECT
public:
  explicit LogEntryEditDialog(const LogbookEntry& entry, const QStringList& projects, const QStringList& steps, QWidget *parent = nullptr);
  ~LogEntryEditDialog();

  LogbookEntry getEntry();

private:
  Ui::LogEntryEditDialog *ui;
  LogbookEntry entry;
};

#endif // LOGENTRYEDITDIALOG_H
