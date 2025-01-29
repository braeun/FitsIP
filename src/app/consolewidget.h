#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <fitsbase/qconsolewidget/qconsolewidget.h>
#include <QWidget>

namespace Ui {
class ConsoleWidget;
}

class ConsoleWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ConsoleWidget(QWidget *parent = nullptr);
  ~ConsoleWidget();

  void setMode(QConsoleWidget::ConsoleMode mode);

  void writeStdOut(const QString& s);

  void writeStdErr(const QString& s);

signals:
  // fired when user hits the return key
  void consoleCommand(const QString& code);

private:
  void handleCommand(const QString& code);
  Ui::ConsoleWidget *ui;
};

#endif // CONSOLEWIDGET_H
