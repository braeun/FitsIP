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
#include <fitsbase/fitsobject.h>
#include <fitsbase/math/moments.h>
#include <iomanip>
#include <iostream>
#include <sstream>

AnalyseProfileDialog::AnalyseProfileDialog(QWidget *parent):QDialog(parent),
  ui(new Ui::AnalyseProfileDialog),
  object(nullptr)
{
  ui->setupUi(this);
}

AnalyseProfileDialog::~AnalyseProfileDialog()
{
  delete ui;
}

void AnalyseProfileDialog::setImage(FitsObject* obj)
{
  object = obj;
  ui->horizontalProfileChart->plot(obj->getXProfile(),false);
  ui->verticalProfileChart->plot(obj->getYProfile(),true);
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
