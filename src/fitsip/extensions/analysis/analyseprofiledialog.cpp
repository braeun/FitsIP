/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to analyse a profile                                         *
 *                                                                              *
 * modified: 2025-06-07                                                         *
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

#include "analyseprofiledialog.h"
#include "ui_analyseprofiledialog.h"
#include <fitsip/core/fitsobject.h>
#include <fitsip/core/math/gaussfit.h>
#include <fitsip/core/math/mathfunctions.h>
#include <fitsip/core/math/moments.h>
#include <fitsip/core/widgets/profilechart.h>
#include <QMessageBox>
#include <iomanip>
#include <iostream>
#include <sstream>

AnalyseProfileDialog::AnalyseProfileDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::AnalyseProfileDialog),
  object(nullptr)
{
  ui->setupUi(this);
  verticalProfileChart = new ProfileChart(ui->widget);
  verticalProfileChart->setObjectName(QString::fromUtf8("verticalProfileChart"));
  verticalProfileChart->setFrameShape(QFrame::StyledPanel);
  verticalProfileChart->setFrameShadow(QFrame::Raised);
  ui->gridLayout->addWidget(verticalProfileChart, 3, 1, 1, 1);
  horizontalProfileChart = new ProfileChart(ui->widget);
  horizontalProfileChart->setObjectName(QString::fromUtf8("horizontalProfileChart"));
  QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(horizontalProfileChart->sizePolicy().hasHeightForWidth());
  horizontalProfileChart->setSizePolicy(sizePolicy1);
  horizontalProfileChart->setFrameShape(QFrame::StyledPanel);
  horizontalProfileChart->setFrameShadow(QFrame::Raised);
  ui->gridLayout->addWidget(horizontalProfileChart, 3, 0, 1, 1);

  connect(ui->fitGaussButton,&QPushButton::clicked,this,[this](){fitGauss();});
}

AnalyseProfileDialog::~AnalyseProfileDialog()
{
  delete ui;
}

void AnalyseProfileDialog::setImage(FitsObject* obj)
{
  object = obj;
  horizontalProfileChart->plot(obj->getXProfile(),false);
  verticalProfileChart->plot(obj->getYProfile(),true);
  updateMoments();
}

void AnalyseProfileDialog::done(int r)
{
//  std::cout << "AnalyseProfileDialog::done" << std::endl;
  object = nullptr;
  QDialog::done(r);
}




double AnalyseProfileDialog::getBackground(const Profile& profile)
{
  int n = 0;
  double sum = 0;
  if (profile.size() < 15) return 0;
  for (int i=0;i<5;i++)
  {
    sum += profile[i].y();
    sum += profile[profile.size()-i-1].y();
    n += 2;
  }
  return sum / n;
}

void AnalyseProfileDialog::updateMoments()
{
  if (!object) return;
  {
    double bkg = getBackground(object->getXProfile());
    Moments m(object->getXProfile(),4*bkg);
    std::ostringstream s;
    s.precision(1);
    s.setf(std::ios::fixed);
    s << "background:" << std::setw(15) << bkg << std::endl;
    s << "center    :" << std::setw(15) << m.getCenter() << " px" << std::endl;
    s << "sigma     :" << std::setw(15) << m.getStandardDeviation() << " px" << std::endl;
    s << "skewness  :" << std::setw(15) << m.getSkewness() << std::endl;
    s << "kurtosis  :" << std::setw(15) << m.getKurtosis() << std::endl;
    ui->horizontalDataField->setPlainText(QString::fromStdString(s.str()));
  }
  {
    double bkg = getBackground(object->getYProfile());
    Moments m(object->getYProfile(),4*bkg);
    std::ostringstream s;
    s.precision(1);
    s.setf(std::ios::fixed);
    s << "background:" << std::setw(15) << bkg << std::endl;
    s << "center    :" << std::setw(15) << m.getCenter() << " px" << std::endl;
    s << "sigma     :" << std::setw(15) << m.getStandardDeviation() << " px" << std::endl;
    s << "skewness  :" << std::setw(15) << m.getSkewness() << std::endl;
    s << "kurtosis  :" << std::setw(15) << m.getKurtosis() << std::endl;
    ui->verticalDataField->setPlainText(QString::fromStdString(s.str()));
  }
}

void AnalyseProfileDialog::fitGauss()
{
  if (!object) return;
  GaussFit fit;
  std::vector<ValueType> x;
  std::vector<ValueType> y;
  for (const QPointF& p : object->getXProfile())
  {
    x.push_back(p.x());
    y.push_back(p.y());
  }
  if (fit.fit(x,y) == 0)
  {
    ValueType center = fit.getCenter();
    ValueType sigma = fit.getSigma();
    ValueType amp = fit.getAmplitude();
    std::ostringstream s;
    s.precision(1);
    s.setf(std::ios::fixed);
    s << "Gauss Fit" << std::endl;
    s << "  center   :" << std::setw(15) << center << " px" << std::endl;
    s << "  sigma    :" << std::setw(15) << sigma << " px" << std::endl;
    s << "  amplitude:" << std::setw(15) << amp << std::endl;
    ui->horizontalDataField->append(QString::fromStdString(s.str()));
    QVector<QPointF> curve;
    for (const QPointF& p : object->getXProfile())
    {
      curve.push_back(QPointF(p.x(),math_functions::gaussian(p.x(),amp,center,sigma)));
    }
    horizontalProfileChart->plotOverlay(curve);
  }
  else
  {
    QMessageBox::information(this,"Analyse Profiles","Fit failed!");
  }
  x.clear();
  y.clear();
  for (const QPointF& p : object->getYProfile())
  {
    x.push_back(p.x());
    y.push_back(p.y());
  }
  if (fit.fit(x,y) == 0)
  {
    ValueType center = fit.getCenter();
    ValueType sigma = fit.getSigma();
    ValueType amp = fit.getAmplitude();
    std::ostringstream s;
    s.precision(1);
    s.setf(std::ios::fixed);
    s << "Gauss Fit" << std::endl;
    s << "  center   :" << std::setw(15) << center << " px" << std::endl;
    s << "  sigma    :" << std::setw(15) << sigma << " px" << std::endl;
    s << "  amplitude:" << std::setw(15) << amp << std::endl;
    ui->verticalDataField->append(QString::fromStdString(s.str()));
    QVector<QPointF> curve;
    for (const QPointF& p : object->getYProfile())
    {
      curve.push_back(QPointF(p.x(),math_functions::gaussian(p.x(),amp,center,sigma)));
    }
    verticalProfileChart->plotOverlay(curve);
  }
  else
  {
    QMessageBox::information(this,"Analyse Profiles","Fit failed!");
  }
}
