#ifndef PROFILECHART_H
#define PROFILECHART_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

class Profile;
class QwtPlotGrid;
class QwtPlotPicker;
class QwtPlotZoomer;

class ProfileChart: public QwtPlot
{
public:
  ProfileChart(QWidget* parent=nullptr);

  void plot(const Profile& profile, bool vertical);

  void setLogScale(bool flag);

private:
  QwtPlotCurve* profile;
  QwtPlotMarker* marker;
  QwtPlotGrid* grid;
  QwtPlotPicker* picker;
};

#endif // PROFILECHART_H
