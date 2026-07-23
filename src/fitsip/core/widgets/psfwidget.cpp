/********************************************************************************
 *                                                                              *
 * FitsIP - point spread function parameter widget                              *
 *                                                                              *
 * modified: 2025-05-30                                                         *
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

#include "psfwidget.h"
#include "ui_psfwidget.h"
#include "../psf/psf.h"
#include "../psf/psffactory.h"

PSFWidget::PSFWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::PSFWidget)
{
  ui->setupUi(this);
  connect(ui->functionBox,&QComboBox::currentTextChanged,this,[this](const QString& text){selectionChanged(text);});
  updatePSFList();
}

PSFWidget::~PSFWidget()
{
  delete ui;
}

void PSFWidget::updatePSFList()
{
  int index = -1;
  ui->functionBox->clear();
  int i = 0;
  for (const auto& psf : PSFFactory::getInstance()->getList())
  {
    ui->functionBox->addItem(psf->getName());
    if (lastFunction == psf->getName())
    {
      index = i;
    }
    ++i;
  }
  if (index >= 0)
  {
    ui->functionBox->setCurrentIndex(index);
    selectionChanged(lastFunction);
    if (ui->parameterTableWidget->rowCount() == lastParameters.size())
    {
      for (int row=0;row<ui->parameterTableWidget->rowCount();++row)
      {
        ui->parameterTableWidget->item(row,1)->setText(QString::number(lastParameters[row]));
      }
    }
  }
}



QString PSFWidget::getFunction()
{
  lastFunction = ui->functionBox->currentText();
  return lastFunction;
}

std::vector<ValueType> PSFWidget::getParameters()
{
  lastParameters.clear();
  for (int i=0;i<ui->parameterTableWidget->rowCount();i++)
  {
    lastParameters.push_back(ui->parameterTableWidget->item(i,1)->text().toDouble());
  }
  return lastParameters;
}




void PSFWidget::selectionChanged(const QString &text)
{
  ui->parameterTableWidget->clearContents();
  const PSF *psf = PSFFactory::getInstance()->getPSF(text);
  if (psf)
  {
    std::vector<QString> parameters = psf->getParameterNames();
    ui->parameterTableWidget->setRowCount(parameters.size());
    for (size_t i=0;i<parameters.size();i++)
    {
      QTableWidgetItem* item = new QTableWidgetItem(parameters[i]);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable); // non editable
      ui->parameterTableWidget->setItem(i,0,item);
      ui->parameterTableWidget->setItem(i,1,new QTableWidgetItem("0.0"));
    }
    emit functionSelected(text);
  }
}


