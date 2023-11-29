#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "InitialAdviceEvent.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString lit_style = "background: rgb(248,228,92);";
    QString unlit_style = "background: rgb(30,30,30);";
    ui->lamp1->setStyleUnlit(unlit_style);
    ui->lamp1->setStyleLit(lit_style);
    ui->lamp1->turnOff();
    ui->lamp2->setStyleUnlit(unlit_style);
    ui->lamp2->setStyleLit(lit_style);
    ui->lamp2->turnOff();
    ui->lamp3->setStyleUnlit(unlit_style);
    ui->lamp3->setStyleLit(lit_style);
    ui->lamp3->turnOff();
    ui->lamp4->setStyleUnlit(unlit_style);
    ui->lamp4->setStyleLit(lit_style);
    ui->lamp4->turnOff();

    repaint();

    seq = new aed::EventSequence();
    seq->add(new aed::InitialAdviceEvent("InitialAdvice: Stay calm", INITIAL_ADVICE_DELAY, "STAY CALM", ui->lamp1));
    seq->add(new aed::InitialAdviceEvent("InitialAdvice: Check responsiveness", INITIAL_ADVICE_DELAY, "CHECK RESPONSIVENESS", ui->lamp2));
    seq->add(new aed::InitialAdviceEvent("InitialAdvice: Call for help", INITIAL_ADVICE_DELAY, "CALL FOR HELP", ui->lamp3));
    seq->add(new aed::InitialAdviceEvent("InitialAdvice: Attach pads", aed::SequencedEvent::NOTIMER, "ATTACH DEFIB PADS TO PATIENT'S BARE CHEST", ui->lamp4));

    for(int i = 0; i < seq->getLength(); i++)
    {
        eventHandles.append(seq->getAt(i));
    }

    connect(this, SIGNAL(startSequence()), seq, SLOT(start()));
    connect(ui->padsButton, SIGNAL(pressed()), seq, SLOT(stop()));

    emit startSequence();

    ui->testLabel->setText("hoohaa");
    lcdDisplay = new LCDDisplay(ui->ecgGraph, ui->testLabel);
    ecgModule = new aed::ModuleECGAssessment(lcdDisplay);

    connect(ui->beginECGButton, SIGNAL(pressed()), ecgModule, SLOT(startAssessment()));
    connect(ui->stopECGButton, SIGNAL(pressed()), ecgModule, SLOT(endAssessment()));
    connect(ui->tachyButton, SIGNAL(pressed()), ecgModule, SLOT(setTachyRhythm()));
    connect(ui->fibButton, SIGNAL(pressed()), ecgModule, SLOT(setRhythm(VENT_FIB)));
    connect(ui->nonShockableButton, SIGNAL(pressed()), ecgModule, SLOT(setRhythm(NON_SHOCKABLE)));
    connect(lcdDisplay, SIGNAL(plotGraphSignal()), this, SLOT(plotGraph()));
    connect(lcdDisplay, SIGNAL(clearGraphSignal()), this, SLOT(clearGraph()));

//    QVector<double> x(5), y(5);
//    for (int i = 0; i < 5; i++) {
//        x[i] = i;
//        y[i] = i;
//    }

//    ui->graph->addGraph();
//    ui->graph->graph(0)->setData(x, y);
//    ui->graph->xAxis->setRange(0, 22);
//    ui->graph->yAxis->setRange(0, 5);

    ecgModule->startAssessment();
    ui->ecgGraph->addGraph();
    ui->ecgGraph->graph(0)->setData(lcdDisplay->getGraphXData(), lcdDisplay->getGraphYData());
    ui->ecgGraph->xAxis->setRange(0, 22);
    ui->ecgGraph->yAxis->setRange(0, 5);

}

MainWindow::~MainWindow()
{

    delete ui;
    if (lcdDisplay !=  nullptr) delete lcdDisplay;
    if (ecgModule !=  nullptr) delete ecgModule;
}

void MainWindow::plotGraph()
{

      QVector<double> x(5), y(5);
      for (int i = 0; i < 5; i++) {
        x[i] = i;
        y[i] = i;
      }

      ui->ecgGraph->addGraph();
      ui->ecgGraph->graph(0)->setData(x, y);
      ui->ecgGraph->xAxis->setRange(0, 22);
      ui->ecgGraph->yAxis->setRange(0, 5);
//    ui->ecgGraph->clearGraphs();
//    ui->ecgGraph->addGraph();
//    ui->ecgGraph->graph(0)->setData(lcdDisplay->getGraphXData(), lcdDisplay->getGraphYData());
//    ui->ecgGraph->xAxis->setRange(0, 22);
//    ui->ecgGraph->yAxis->setRange(0, 5);
//    qDebug() << "GRAPH DATA PLOTTED";
//    qDebug() << QString("%1").arg(lcdDisplay->getGraphXData().at(0));
//    qDebug() << QString("%1").arg(lcdDisplay->getGraphYData().at(0));
//    qDebug() << QString("%1").arg(lcdDisplay->getGraphXData().at(1));
//    qDebug() << QString("%1").arg(lcdDisplay->getGraphYData().at(1));
}

void MainWindow::clearGraph()
{
    ui->ecgGraph->clearGraphs();
    qDebug() << "GRAPH DATA CLEAREeD";
}


void MainWindow::quitProgram()
{
    for(int i = 0; i < eventHandles.size(); i++)
    {
        if(eventHandles[i] != nullptr) eventHandles[i]->deleteLater();
    }
    QApplication::quit();
}
