#ifndef OPCOMBINECHANNELSDIALOG_H
#define OPCOMBINECHANNELSDIALOG_H

#include <QDialog>
#include <imagecollection.h>

namespace Ui {
class OpCombineChannelsDialog;
}

class OpCombineChannelsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpCombineChannelsDialog(QWidget *parent = nullptr);
  ~OpCombineChannelsDialog();

  void setCollection(const ImageCollection& collection);

  int getRedImage() const;

  int getGreenImage() const;

  int getBlueImage() const;

private:
  Ui::OpCombineChannelsDialog *ui;
};

#endif // OPCOMBINECHANNELSDIALOG_H
