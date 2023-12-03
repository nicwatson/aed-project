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

    QString lit_style = "background: rgb(248,228,92); border-radius: 8px;";
    QString unlit_style = "background: rgb(132,130,132); border-radius: 8px;";
    ui->lamp_CheckResp->setStyleUnlit(unlit_style);
    ui->lamp_CheckResp->setStyleLit(lit_style);
    ui->lamp_CheckResp->turnOff();
    ui->lamp_CallHelp->setStyleUnlit(unlit_style);
    ui->lamp_CallHelp->setStyleLit(lit_style);
    ui->lamp_CallHelp->turnOff();
    ui->lamp_AttPads->setStyleUnlit(unlit_style);
    ui->lamp_AttPads->setStyleLit(lit_style);
    ui->lamp_AttPads->turnOff();
    ui->lamp_Analysing->setStyleUnlit(unlit_style);
    ui->lamp_Analysing->setStyleLit(lit_style);
    ui->lamp_Analysing->turnOff();
    ui->lamp_CPR->setStyleUnlit(unlit_style);
    ui->lamp_CPR->setStyleLit(lit_style);
    ui->lamp_CPR->turnOff();

    repaint();


    aedGui::LCDDisplayParams params = {
        .lcdDisplayFrame = ui->LCDDisplay,
        .ecgGraph = ui->ecgGraph,
        .promptLabel = ui->LCDPrompt,
        .helpLabel = ui->LCDHelp,
        .shockCounter = ui->LCDShocks,
        .timer = ui->LCDTimer,
        .compressionDepthBar = ui->compressionDepthBar
    };
    // 1. INSTNATIATE MODULES
    // Instantiating LCDDisplay and all modules
    LCDDisplay = new aedGui::LCDDisplay(params);
    ecgModule = new aedModel::ModuleECGAssessment(LCDDisplay);

    connect(ui->beginECGButton, SIGNAL(pressed()), ecgModule, SLOT(startAssessment()));
    connect(ui->stopECGButton, SIGNAL(pressed()), ecgModule, SLOT(endAssessment()));
    connect(ui->tachyButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_TACHY);} );
    connect(ui->fibButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_FIB);} );
    connect(ui->nonShockableButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::NON_SHOCKABLE);} );
}

MainWindow::~MainWindow()
{

    delete ui;

    if (LCDDisplay !=  nullptr) LCDDisplay->deleteLater();
    if (ecgModule !=  nullptr) ecgModule->deleteLater();
}

void MainWindow::quitProgram()
{
    QApplication::quit();
}

