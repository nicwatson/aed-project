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
    cprHelpModule = new aedModel::ModuleCPRHelp();
    shockModule = new aedModel::ModuleShock();

    // Connecting GUI buttons to ecg module
    connect(ui->beginECGButton, SIGNAL(pressed()), ecgModule, SLOT(startAssessment()));
    connect(ui->stopECGButton, SIGNAL(pressed()), ecgModule, SLOT(endAssessment()));
    connect(ui->tachyButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_TACHY);} );
    connect(ui->fibButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_FIB);} );
    connect(ui->nonShockableButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::NON_SHOCKABLE);} );

    // Connecting GUI buttons to cpr help module
    connect(ui->cprDepthSlider, SIGNAL(valueChanged(int)), cprHelpModule, SLOT(updateCompressionDepth(int)));
    connect(ui->cprRateSlider, SIGNAL(valueChanged(int)), cprHelpModule, SLOT(updateCompressionRate(int)));
    connect(ui->toggleCompressionsButton, SIGNAL(clicked(bool)), cprHelpModule, SLOT(toggleCompressions(bool))); // checked, working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCompressionsStarted, [=]() {
        ui->toggleCompressionsButton->setText("Stop Compressions");
        ui->toggleCompressionsButton->setChecked(true);
    }); // checked- working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCompressionsStopped, [=]() {
        ui->toggleCompressionsButton->setText("Start Compressions");
        ui->toggleCompressionsButton->setChecked(false);
    }); // checked- working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRCompressionRatePrompt, ui->LCDHelp, [this](QString msg) {ui->LCDHelp->setText(msg); }); // checked- working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalDisplayCompressionDepth, ui->compressionDepthBar, [this](int val) {
        qDebug() << QString("Old value is %1").arg(val);
        val = (val * 100) / 24; // convert from slider scale of (0-24) to progress bar scale of (0-100)
        ui->compressionDepthBar->setValue(val);
        qDebug() << QString("Vvalue is %1").arg(val);
    }); // Checked- working

    // Connect GUI Elements to ModuleShock (and vice versa)
    QString shockLitStyle = "image: url(:/shockButton/ShockButton_ButtonPressed.svg); border: 0;";
    QString shockUnlitStyle = "image: url(:/shockButton/ShockButton_Button.svg); border: 0;";
    connect(ui->shockButton, &QPushButton::pressed, shockModule, [=]() {
        shockModule->shockButtonPressed();
        ui->shockButton->setStyleSheet(shockLitStyle);
    });
    connect(ui->shockButton, &QPushButton::released, shockModule, [=]() {
        shockModule->shockButtonReleased();
        ui->shockButton->setStyleSheet(shockUnlitStyle);
    });
    connect(shockModule, &aedModel::ModuleShock::signalCharged, [=]() {
        shockButtonFlashTimer.setSingleShot(false);
        shockButtonFlashTimer.setInterval(500);
        connect(&shockButtonFlashTimer, &QTimer::timeout, this, &MainWindow::toggleShockButtonFlash);
        shockButtonFlashTimer.start();
    });
    connect(shockModule, &aedModel::ModuleShock::signalAborted, [=]() {
        shockButtonFlashTimer.stop();
        disconnect(&shockButtonFlashTimer, &QTimer::timeout, this, &MainWindow::toggleShockButtonFlash);
        ui->shockButton->setStyleSheet(shockUnlitStyle);
    });

//    connect(ui->shockButton, SIGNAL(pressed()), shockModule, SLOT(shockButtonPressed())); // Checked- working
//    connect(ui->shockButton, SIGNAL(released()), shockModule, SLOT(shockButtonReleased())); // Checked- working
//    connect(ui->shockButton, SIGNAL(pressed()), shockModule, SLOT(shockButtonPressed())); // Checked- working
//    connect(ui->shockButton, SIGNAL(released()), shockModule, SLOT(shockButtonReleased())); // Checked- working




    // 2. CONNECT GUI BUTTONS TO MODULES (AND VICE VERSA)

    // 3. CONNECT GUI ELEMENTS TO AED (AND VICE VERSA)
    connect(ui->powerButton, SIGNAL(pressed()), aed, SLOT(togglePowerButton()));
    connect(ui->adultPadsButton, SIGNAL(clicked()), aed, SLOT(plugCableAdult()));
    connect(ui->pediatricPadsButton, SIGNAL(clicked()), aed, SLOT(plugCableChild()));
    connect(ui->unpluggedCablesButton, SIGNAL(clicked()), aed, SLOT(unplugCable()));
    connect(ui->attachedPadsButton, &QRadioButton::clicked, [=]() {aed->attachPads(true);} ); // checked- working
    connect(ui->detachedPadsButton, &QRadioButton::clicked, [=]() {aed->attachPads(false);} ); // checked- working
    connect(ui->rechargeBatteryButton, SIGNAL(pressed()), aed, SLOT(changeBatteries()));
    connect(aed, &aedModel::AED::signalUserPrompt, ui->LCDPrompt, [this](QString msg) {ui->LCDPrompt->setText(msg); }); // checked- working, I called a dummy AED method which emitted signalUserPrompt("dumbo")


    aed->dummy();
    cprHelpModule->dummy();
    shockModule->dummy();


    // 4. ADD MODULES TO AED







}

MainWindow::~MainWindow()
{

    delete ui;

    if (LCDDisplay !=  nullptr) LCDDisplay->deleteLater();
    if (ecgModule !=  nullptr) ecgModule->deleteLater();
}

void MainWindow::toggleShockButtonFlash()
{
    QString shockLitStyle = "image: url(:/shockButton/ShockButton_ButtonPressed.svg); border: 0;";
    QString shockUnlitStyle = "image: url(:/shockButton/ShockButton_Button.svg); border: 0;";
    if (ui->shockButton->styleSheet() == shockLitStyle) {
        ui->shockButton->setStyleSheet(shockUnlitStyle);
    } else {
        ui->shockButton->setStyleSheet(shockLitStyle);
    }
}

void MainWindow::quitProgram()
{
    QApplication::quit();
}

