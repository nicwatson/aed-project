#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "event/PictogramAdviceEvent.h"

#include <QKeyEvent>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    numKeysDown = 0;

    aed = new aedModel::AED();

    QString lit_style = "background: rgb(248,228,92);";
    QString unlit_style = "background: rgb(30,30,30);";
    ui->lamp_CheckResp->setStyleUnlit(unlit_style);
    ui->lamp_CheckResp->setStyleLit(lit_style);
    ui->lamp_CheckResp->turnOff();
    ui->lamp_CallHelp->setStyleUnlit(unlit_style);
    ui->lamp_CallHelp->setStyleLit(lit_style);
    ui->lamp_CallHelp->turnOff();
    ui->lamp_AttPads->setStyleUnlit(unlit_style);
    ui->lamp_AttPads->setStyleLit(lit_style);
    ui->lamp_AttPads->turnOff();

    repaint();

    startupSequence = new aedModel::ModuleStartupAdvice(ui->lamp_CheckResp, ui->lamp_CallHelp, ui->lamp_AttPads);

    aed->addModuleStartupAdvice(startupSequence);
    connect(ui->padsButton, SIGNAL(pressed()), aed, SLOT(notifyPadsAttached()));


    //aed->doStartupAdvice();
    /*
    connect(this, SIGNAL(startSequence()), startupSequence, SLOT(start()));
    connect(ui->padsButton, SIGNAL(pressed()), startupSequence, SLOT(stop()));

    startupSequence->startFromBeginning();*/


    lcdDisplay = new aedGui::LCDDisplay(ui->ecgGraph, ui->lcdPrompt);
    ecgModule = new aedModel::ModuleECGAssessment(lcdDisplay);

    connect(ui->beginECGButton, SIGNAL(pressed()), ecgModule, SLOT(startAssessment()));
    connect(ui->stopECGButton, SIGNAL(pressed()), ecgModule, SLOT(endAssessment()));
    connect(ui->tachyButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_TACHY);} );
    connect(ui->fibButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_FIB);} );
    connect(ui->nonShockableButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::NON_SHOCKABLE);} );


}

MainWindow::~MainWindow()
{

    delete ui;

    if (lcdDisplay !=  nullptr) lcdDisplay->deleteLater();
    if (ecgModule !=  nullptr) ecgModule->deleteLater();
}

void MainWindow::quitProgram()
{
    QApplication::quit();
}


void MainWindow::keyPressEvent(QKeyEvent * e)
{
    /*if(e->text().contains(acceptedKeys))
    {
        e->accept();
        ++numKeysDown;
        qDebug() << "Accepted key pressed: " << e->text() << " : Now " << QString::number(numKeysDown) << " keys pressed." << Qt::endl;
    }*/
}

void MainWindow::keyReleaseEvent(QKeyEvent * e)
{
    /*if(e->text().contains(acceptedKeys))
    {
        e->accept();
        --numKeysDown;
        qDebug() << "Accepted key released: " << e->text() << " : Now " << QString::number(numKeysDown) << " keys pressed." << Qt::endl;
    }*/
}
