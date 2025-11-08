#ifndef METADATATABLEWIDGET_H
#define METADATATABLEWIDGET_H

#include <fitsip/core/fitsobject.h>
#include <QWidget>
#include <memory>


namespace Ui {
class MetadataTableWidget;
}

class ImageMetadata;

class MetadataTableWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MetadataTableWidget(QWidget *parent = nullptr);
  ~MetadataTableWidget();

  void clear();

  void setData(const ImageMetadata& data);

//  void setFile(std::shared_ptr<FitsObject> file);

private:
  Ui::MetadataTableWidget *ui;
};

#endif // METADATATABLEWIDGET_H
