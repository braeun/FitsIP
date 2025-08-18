#ifndef XYCHARTSWIDGET_H
#define XYCHARTSWIDGET_H

#include "../fitsobject.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QWidget>

namespace Ui {
class XYChartsWidget;
}

class XYChartsWidget : public QWidget
{
  Q_OBJECT
public:
  explicit XYChartsWidget(QWidget *parent = nullptr);
  ~XYChartsWidget();

  void setFitsObject(const std::shared_ptr<FitsObject>& o);

private:
  void dataSelected(const QModelIndex& index);

  Ui::XYChartsWidget *ui;
  QwtPlot* plot;
  QwtPlotCurve* curve;
  std::shared_ptr<FitsObject> obj;
};

#endif // XYCHARTSWIDGET_H
