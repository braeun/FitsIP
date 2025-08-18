#ifndef LOGBOOKEXPORTDIALOG_H
#define LOGBOOKEXPORTDIALOG_H

#include <QDialog>
#include <vector>

namespace Ui {
class LogbookExportDialog;
}

class LogbookExportDialog : public QDialog
{
  Q_OBJECT
public:
  explicit LogbookExportDialog(QWidget *parent = nullptr);
  ~LogbookExportDialog();

  QString getFilename() const;

  QString getTemplate() const;

private:
  void browse();
  void browseTemplate();

  Ui::LogbookExportDialog *ui;
  std::vector<QString> templates;
};

#endif // LOGBOOKEXPORTDIALOG_H
