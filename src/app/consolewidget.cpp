#include "consolewidget.h"
#include "ui_consolewidget.h"
#include <iostream>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConsoleWidget)
{
  ui->setupUi(this);
  ui->console->device()->close();
  ui->console->setMode(QConsoleWidget::Input);
  connect(ui->console,&QConsoleWidget::consoleCommand,this,&ConsoleWidget::handleCommand);
}

ConsoleWidget::~ConsoleWidget()
{
  delete ui;
}

void ConsoleWidget::setMode(QConsoleWidget::ConsoleMode mode)
{
  ui->console->setMode(mode);
}

void ConsoleWidget::writeStdOut(const QString& s)
{
  ui->console->writeStdOut(s);
}

void ConsoleWidget::writeStdErr(const QString& s)
{
  ui->console->writeStdErr(s);
}



void ConsoleWidget::handleCommand(const QString& code)
{
  emit consoleCommand(code);
}
