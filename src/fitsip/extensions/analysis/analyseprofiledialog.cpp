/********************************************************************************
 *                                                                              *
 * FitsIP - dialog to analyse a profile                                         *
 *                                                                              *
 * modified: 2025-01-12                                                         *
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
#include <fitsip/core/math/moments.h>
#include <fitsip/core/widgets/profilechart.h>
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
  ui->gridLayout->addWidget(verticalProfileChart, 2, 1, 1, 1);
  horizontalProfileChart = new ProfileChart(ui->widget);
  horizontalProfileChart->setObjectName(QString::fromUtf8("horizontalProfileChart"));
  QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(horizontalProfileChart->sizePolicy().hasHeightForWidth());
  horizontalProfileChart->setSizePolicy(sizePolicy1);
  horizontalProfileChart->setFrameShape(QFrame::StyledPanel);
  horizontalProfileChart->setFrameShadow(QFrame::Raised);
  ui->gridLayout->addWidget(horizontalProfileChart, 2, 0, 1, 1);
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
  {
    Moments m(obj->getXProfile());
    std::ostringstream s;
    s.precision(1);
    s.setf(std::ios::fixed);
    s << "center  :" << std::setw(15) << m.getCenter() << " px" << std::endl;
    s << "sigma   :" << std::setw(15) << m.getStandardDeviation() << " px" << std::endl;
    s << "skewness:" << std::setw(15) << m.getSkewness() << std::endl;
    s << "kurtosis:" << std::setw(15) << m.getKurtosis() << std::endl;
    ui->horizontalDataField->setPlainText(QString::fromStdString(s.str()));
  }
  {
    Moments m(obj->getYProfile());
    std::ostringstream s;
    s.precision(1);
    s.setf(std::ios::fixed);
    s << "center  :" << std::setw(15) << m.getCenter() << " px" << std::endl;
    s << "sigma   :" << std::setw(15) << m.getStandardDeviation() << " px" << std::endl;
    s << "skewness:" << std::setw(15) << m.getSkewness() << std::endl;
    s << "kurtosis:" << std::setw(15) << m.getKurtosis() << std::endl;
    ui->verticalDataField->setPlainText(QString::fromStdString(s.str()));
  }
}

void AnalyseProfileDialog::done(int r)
{
//  std::cout << "AnalyseProfileDialog::done" << std::endl;
  object = nullptr;
  QDialog::done(r);
}
