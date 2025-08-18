#include "xychartswidget.h"
#include "ui_xychartswidget.h"
#include <qwt/qwt_symbol.h>

XYChartsWidget::XYChartsWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::XYChartsWidget)
{
  ui->setupUi(this);
  plot = new QwtPlot(this);
  ui->chartWidgetLayout->addWidget(plot);
  curve = new QwtPlotCurve("Data");
  curve->setPen(palette().color(QPalette::WindowText));
  curve->attach(plot);

  connect(ui->dataListWidget,&QListWidget::doubleClicked,this,&XYChartsWidget::dataSelected);
}

XYChartsWidget::~XYChartsWidget()
{
  delete ui;
}

void XYChartsWidget::setFitsObject(const std::shared_ptr<FitsObject>& o)
{
  obj = o;
  ui->dataListWidget->clear();
  for (const XYData& data : o->getXYData())
  {
    ui->dataListWidget->addItem(data.getTitle());
  }
}

void XYChartsWidget::dataSelected(const QModelIndex& index)
{
  QVector<QPointF> points;
  const XYData& data = obj->getXYData()[index.row()];
  for (size_t i=0;i<data.getX().size();++i)
  {
    QPointF p(data.getX()[i],data.getY()[i]);
    points.push_back(p);
  }
  curve->setSamples(points);
  switch (data.getType())
  {
    case XYData::Curve:
      curve->setStyle(QwtPlotCurve::Lines);
      break;
    case XYData::Scatter:
      curve->setStyle(QwtPlotCurve::NoCurve);
      curve->setSymbol(new QwtSymbol(QwtSymbol::Rect));
      break;
  }

  plot->replot();
}
