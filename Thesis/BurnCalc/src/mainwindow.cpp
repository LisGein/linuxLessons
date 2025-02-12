#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mixture.h"
#include "chemicalSubstance.h"
#include "reaction.h"
#include "gui/guisubstance.h"
#include "gui/guiempericalformula.h"
#include <QHBoxLayout>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent), ui(new Ui::MainWindow)
   , substances(new GuiEmpericalFormula())
{
   ui->setupUi(this);

   QWidget *centralWidget = new QWidget(this);
   QGridLayout *layout = new QGridLayout();
   centralWidget->setLayout(layout);

   layout->addWidget(substances);

   setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
}
