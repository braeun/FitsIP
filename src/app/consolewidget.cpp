#include "consolewidget.h"
#include "ui_consolewidget.h"
#include <iostream>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConsoleWidget)
{
  ui->setupUi(this);
  console = new QConsoleWidget();
  ui->verticalLayout->addWidget(console);
  console->device()->close();
  QTextDocument *doc = console->document();
  QFont font = doc->defaultFont();
  font.setFamily("Courier");
  doc->setDefaultFont(font);
  console->writeStdOut("> ");
  console->setMode(QConsoleWidget::Input);
  connect(console,&QConsoleWidget::consoleCommand,this,&ConsoleWidget::handleCommand);
}

ConsoleWidget::~ConsoleWidget()
{
  delete ui;
}

void ConsoleWidget::setMode(QConsoleWidget::ConsoleMode mode)
{
  if (mode == QConsoleWidget::Input) console->writeStdOut("> ");
  console->setMode(mode);
}

void ConsoleWidget::writeStdOut(const QString& s)
{
  console->writeStdOut(s);
}

void ConsoleWidget::writeStdErr(const QString& s)
{
  console->writeStdErr(s);
}



void ConsoleWidget::handleCommand(const QString& code)
{
  emit consoleCommand(code);
}
