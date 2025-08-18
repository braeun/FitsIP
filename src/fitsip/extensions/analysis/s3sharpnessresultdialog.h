#ifndef S3SHARPNESSRESULTDIALOG_H
#define S3SHARPNESSRESULTDIALOG_H

#include <QDialog>
#include <vector>

class QFileInfo;
class S3SharpnessData;

namespace Ui {
class S3SharpnessResultDialog;
}

class S3SharpnessResultDialog : public QDialog
{
  Q_OBJECT

public:
  explicit S3SharpnessResultDialog(QWidget *parent = nullptr);
  ~S3SharpnessResultDialog();

  void setResult(const std::vector<S3SharpnessData>& entries);

  std::vector<QFileInfo> getFileList() const;

signals:
  void writeToLogbook();
  void fileListChanged();

private:
  void removeRows();

  void save();

  void toLogbook();

  Ui::S3SharpnessResultDialog *ui;
  std::vector<S3SharpnessData> entries;
};

#endif // S3SHARPNESSRESULTDIALOG_H
