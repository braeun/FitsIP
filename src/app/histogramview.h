/********************************************************************************
 *                                                                              *
 * FitsIP - widget containing the histogram and associated controls             *
 *                                                                              *
 * modified: 2022-11-26                                                         *
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

  int32_t getImageScale() const;

signals:

  void imageScaleChanged(double min, double max, int32_t scale);

protected:
  void changeEvent(QEvent* event);

private slots:

  void handleZoomed(const QRectF &rect);

  void spanChanged(int min, int max);

  void on_imageMinIntensity_returnPressed();

  void on_imageMaxIntensity_returnPressed();

  void on_imageScaleBox_currentIndexChanged(int index);

private:
  Ui::HistogramView *ui;
  std::shared_ptr<FitsObject> image;
  QwtPlotCurve* grayCurve;
  QwtPlotMarker* minMarker;
  QwtPlotMarker* maxMarker;
  QwtPlotZoomer* zoomer;
  QwtPlotGrid* grid;
  QwtPlotPicker* plotPicker;
  bool updating;
};

#endif // HISTOGRAMVIEW_H
