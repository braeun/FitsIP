/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the histogram and associated controls             *
 *                                                                              *
 * modified: 2025-02-09                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of FitsIP.                                                 *
 * FitsIP is free software: you can redistribute it and/or modify it            *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * FitsIP is distributed in the hope that it will be useful, but                *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * FitsIP. If not, see <https://www.gnu.org/licenses/>.                         *
 ********************************************************************************/

#ifndef HISTOGRAMVIEW_H
#define HISTOGRAMVIEW_H

#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <QWidget>
#include <memory>

class FitsObject;
class QwtPlotGrid;
class QwtPlotPicker;
class QwtPlotZoomer;
class QxtSpanSlider;

namespace Ui {
  class HistogramView;
}

class HistogramView : public QWidget
{
  Q_OBJECT

public:
  explicit HistogramView(QWidget *parent = nullptr);
  ~HistogramView();

  void setImage(std::shared_ptr<FitsObject> obj);

  void redraw();

  int getImageScale() const;

  double getScaleMin() const;

  double getScaleMax() const;

signals:

  void imageScaleChanged(double min, double max, int scale);

protected:
  void changeEvent(QEvent* event);

private:
  void handleZoomed(const QRectF &rect);
  void spanChanged(int min, int max);
  void changeIntensity();
  int toSlider(double v);
  double fromSlider(int v);

  Ui::HistogramView *ui;
  QxtSpanSlider* scalingSlider;
  std::shared_ptr<FitsObject> image;
  QwtPlotCurve* grayCurve;
  QwtPlotMarker* minMarker;
  QwtPlotMarker* maxMarker;
  QwtPlotZoomer* zoomer;
  QwtPlotGrid* grid;
  QwtPlotPicker* plotPicker;
  bool updating;
  double histmin;
  double histmax;
};

#endif // HISTOGRAMVIEW_H
