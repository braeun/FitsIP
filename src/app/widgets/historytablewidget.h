#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <fitsip/core/fitsobject.h>
#include <QWidget>
#include <memory>

class QTableWidget;
class QVBoxLayout;

class HistoryTableWidget : public QWidget
{
  Q_OBJECT
public:
  explicit HistoryTableWidget(QWidget *parent = nullptr);

  void clear();

  void setFile(std::shared_ptr<FitsObject> file);

private:
  QVBoxLayout *verticalLayout;
  QTableWidget *tableWidget;

};

#endif // HISTORYWIDGET_H
