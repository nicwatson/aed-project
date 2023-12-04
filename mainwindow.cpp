#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // 0. INSTANTIATE MAIN AED OBJECT
    aed = new aedModel::AED();

    // 1. PREPARE LAMP WIDGETS
    prepareLampWidgets();

    // 2. INSTANTIATE AED MODULES
    buildModules();

    // 3. CONNECT GUI ELEMENTS TO MODULES (AND VICE VERSA)
    buildModuleConnections();

    // 4. CONNECT GUI ELEMENTS TO AED (AND VICE VERSA)
    buildAEDConnections();
    buildDebugFeatures();

    // 5. ADD MODULES TO AED
    aed->addModuleSelfTest(selfTestModule);
    aed->addModuleStartupAdvice(startupAdviceModule);
    aed->addModuleECG(ecgModule);
    aed->addModuleShock(shockModule);
    aed->addModuleCPR(cprHelpModule);

    // Start with everything turned off
    turnoff();

}

MainWindow::~MainWindow()
{
    delete ui;

    if (LCDDisplay != nullptr) LCDDisplay->deleteLater();
    if (ecgModule != nullptr) ecgModule->deleteLater();
}

void MainWindow::prepareLampWidgets()
{
    // Set up the five pictogram-attached advice lamps

    ui->lamp_CheckResp-> setStyleUnlit  (STYLE_ADVICELAMP_UNLIT);
    ui->lamp_CheckResp-> setStyleLit    (STYLE_ADVICELAMP_LIT);
    ui->lamp_CheckResp-> setFlashTime   (FLASH_TIME_DEFAULT);

    ui->lamp_CallHelp->  setStyleUnlit  (STYLE_ADVICELAMP_UNLIT);
    ui->lamp_CallHelp->  setStyleLit    (STYLE_ADVICELAMP_LIT);
    ui->lamp_CallHelp->  setFlashTime   (FLASH_TIME_DEFAULT);

    ui->lamp_AttPads->   setStyleUnlit  (STYLE_ADVICELAMP_UNLIT);
    ui->lamp_AttPads->   setStyleLit    (STYLE_ADVICELAMP_LIT);
    ui->lamp_AttPads->   setFlashTime   (FLASH_TIME_DEFAULT);

    ui->lamp_Analysing-> setStyleUnlit  (STYLE_ADVICELAMP_UNLIT);
    ui->lamp_Analysing-> setStyleLit    (STYLE_ADVICELAMP_LIT);
    ui->lamp_Analysing-> setFlashTime   (FLASH_TIME_DEFAULT);

    ui->lamp_CPR->       setStyleUnlit  (STYLE_ADVICELAMP_UNLIT);
    ui->lamp_CPR->       setStyleLit    (STYLE_ADVICELAMP_LIT);
    ui->lamp_CPR->       setFlashTime   (FLASH_TIME_DEFAULT);


    // Set up the shock button's flashing light function
    ui->shockButton->    setStyleUnlit (STYLE_SHOCK_UNLIT);
    ui->shockButton->    setStyleLit   (STYLE_SHOCK_LIT);
    ui->shockButton->   setFlashTime   (FLASH_TIME_SHOCK);

    repaint();
}



void MainWindow::buildModules()
{
    // Instantiating LCDDisplay and all modules
    aedGui::LCDDisplayParams params = {
        .lcdDisplayFrame = ui->LCDDisplay,
        .ecgGraph = ui->ecgGraph,
        .promptLabel = ui->lcdPrompt,
        .helpLabel = ui->lcdCPRHelp,
        .shockCounter = ui->lcdShocks,
        .timer = ui->lcdTimer,
        .compressionDepthBar = ui->compressionDepthBar
    };
    LCDDisplay = new aedGui::LCDDisplay(params);
    ecgModule = new aedModel::ModuleECGAssessment(LCDDisplay);
    cprHelpModule = new aedModel::ModuleCPRHelp();
    shockModule = new aedModel::ModuleShock();
    selfTestModule = new aedModel::ModuleSelfTest();
    startupAdviceModule = new aedModel::ModuleStartupAdvice(ui->lamp_CheckResp, ui->lamp_CallHelp, ui->lamp_AttPads);
}

void MainWindow::buildModuleConnections()
{
    // CONNECTING GUI ELEMENTS TO ModuleECGAssessment
    connect(ui->tachyButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_TACHY);} );
    connect(ui->fibButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_FIB);} );
    connect(ui->nonShockableButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::NON_SHOCKABLE);} );

    // CONNECTING GUI ELEMENTS TO ModuleShock (and vice-versa)

    // Lights up the Shock button when pressed. Checked- working
    connect(ui->shockButton, &QPushButton::pressed, shockModule, [=]() {
        shockModule->shockButtonPressed();
        ui->shockButton->turnOn();
    });

    // Darkens the Shock button when released. Checked- working
    connect(ui->shockButton, &QPushButton::released, shockModule, [=]() {
        shockModule->shockButtonReleased();
        ui->shockButton->turnOff();
    });

    // Makes the Shock Button start to flash. Checked- working
    connect(shockModule, &aedModel::ModuleShock::signalCharged, ui->shockButton, &aedGui::LampButton::startFlash);

    // Makes the Shock Button stop flashing. Checked- working
    connect(shockModule, &aedModel::ModuleShock::signalAborted, ui->shockButton, &aedGui::LampButton::stopFlash);
    connect(shockModule, &aedModel::ModuleShock::signalShockDelivered, [=]() {
        ui->shockButton->stopFlash();
    });

    connect(shockModule, SIGNAL(signalShockDelivered(int)), LCDDisplay, SLOT(setShockCounter(int)));

    // CONNECTING GUI ELEMENTS TO ModuleCPRHelp
    connect(ui->cprDepthSlider, SIGNAL(valueChanged(int)), cprHelpModule, SLOT(updateCompressionDepth(int)));
    connect(ui->cprRateSlider, SIGNAL(valueChanged(int)), cprHelpModule, SLOT(updateCompressionRate(int)));

    connect(ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::signalButtonToggleCompressions, cprHelpModule, &aedModel::ModuleCPRHelp::toggleCompressions);
    connect(ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::signalButtonToggleCompressions, [=]() {
        cprHelpModule->updateCompressionDepth(ui->cprDepthSlider->value());
        cprHelpModule->updateCompressionRate(ui->cprRateSlider->value());
    });
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRStarted, ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::enableCPR);
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRComplete, ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::disableCPR);
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRAborted, ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::disableCPR);

    /*
    connect(ui->toggleCompressionsButton, SIGNAL(clicked(bool)), cprHelpModule, SLOT(toggleCompressions(bool))); // checked, working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCompressionsStarted, [=]() {
        ui->toggleCompressionsButton->setText("Stop CPR");
        ui->toggleCompressionsButton->setChecked(true);
    }); // checked- working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCompressionsStopped, [=]() {
        ui->toggleCompressionsButton->setText("Start CPR");
        ui->toggleCompressionsButton->setChecked(false);
    }); // checked- working
    connect(ui->toggleCompressionsButton, &QPushButton::toggled, cprHelpModule, &aedModel::ModuleCPRHelp::toggleCompressions);

    // Connecting the toggleCompressionsButton to be enabled/disabled (and reset) by the CPR module at start/end of CPR phase
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRStarted, [=]() {
        ui->toggleCompressionsButton->setText("Start CPR");
        ui->toggleCompressionsButton->setChecked(false);
        ui->toggleCompressionsButton->setEnabled(true);
    });
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRComplete, [=]() {
        ui->toggleCompressionsButton->setText("Start CPR");
        ui->toggleCompressionsButton->setChecked(false);
        ui->toggleCompressionsButton->setEnabled(false);
    });
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRAborted, [=]() {
        ui->toggleCompressionsButton->setText("Start CPR");
        ui->toggleCompressionsButton->setChecked(false);
        ui->toggleCompressionsButton->setEnabled(false);
    });

    */



    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRCompressionRatePrompt, ui->lcdCPRHelp, [this](QString msg) {ui->lcdCPRHelp->setText(msg); }); // checked- working
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalDisplayCompressionDepth, ui->compressionDepthBar, [this](int val) {
        //qDebug() << QString("Old value is %1").arg(val);
        // val = (val * 100) / CPR_ADULT_DEPTH_MAX; // convert from slider scale of (0-24) to progress bar scale of (0-100) // Unnecessary - set progress bar max to 24 or 25
        ui->compressionDepthBar->setValue(val);
        //qDebug() << QString("Value is %1").arg(val);
    }); // Checked- working


}

void MainWindow::buildAEDConnections()
{
    // CONNECT GUI SIGNALS TO AED SLOTS

    connect(ui->powerButton, &QPushButton::pressed, aed, &aedModel::AED::togglePowerButton);
    connect(ui->adultPadsButton, &QRadioButton::clicked, aed, &aedModel::AED::plugCableAdult);
    connect(ui->pediatricPadsButton, &QRadioButton::clicked, aed, &aedModel::AED::plugCableChild);
    connect(ui->unpluggedCablesButton, &QRadioButton::clicked, aed, &aedModel::AED::unplugCable);
    connect(ui->attachedPadsButton, &QRadioButton::clicked, [=]() {aed->attachPads(true);} ); // checked- working
    connect(ui->detachedPadsButton, &QRadioButton::clicked, [=]() {aed->attachPads(false);} ); // checked- working
    connect(ui->rechargeBatteryButton, &QPushButton::pressed, aed, &aedModel::AED::changeBatteries);


    // CONNECT AED SIGNALS TO GUI SLOTS

    connect(aed, &aedModel::AED::signalUserPrompt, LCDDisplay, &aedGui::LCDDisplay::setPromptLabel); // checked- working

    // Connect AED signals to those pictogram lamps that are not linked to the initial startup advice
    connect(aed, &aedModel::AED::signalStartLampStandback, ui->lamp_Analysing, &aedGui::LampWidget::startFlash); // checked- working
    connect(aed, &aedModel::AED::signalStopLampStandback, ui->lamp_Analysing, &aedGui::LampWidget::stopFlash); // checked- working
    connect(aed, &aedModel::AED::signalStartLampCPR, ui->lamp_CPR, &aedGui::LampWidget::startFlash); // checked- working
    connect(aed, &aedModel::AED::signalStopLampCPR, ui->lamp_CPR, &aedGui::LampWidget::stopFlash); // checked- working
    connect(aed, &aedModel::AED::signalStartLampECG, ui->lamp_Analysing, &aedGui::LampWidget::startFlash); // checked- working
    connect(aed, &aedModel::AED::signalStopLampECG, ui->lamp_Analysing, &aedGui::LampWidget::stopFlash); // checked- working

    // Connect AED power/test/battery status changes to GUI indicators
    connect(aed, &aedModel::AED::signalBatteryChanged, ui->batteryPercentLabel, [this](double newBatt) {ui->batteryPercentLabel->setText(QString("%1").arg((int) (newBatt * 100))); }); // checked- working
    connect(aed, &aedModel::AED::signalStartTest, [this](aedModel::AED*) {LCDDisplay->startLCD();} ); // checked- working
    connect(aed, &aedModel::AED::signalPowerOff, LCDDisplay, &aedGui::LCDDisplay::endLCD);
    connect(aed, &aedModel::AED::signalDisplayPassTest, ui->selfTestIndicator, [=]() { ui->selfTestIndicator->setStyleSheet(STYLE_INDICATOR_PASS); });
    connect(aed, &aedModel::AED::signalUnitFailed, ui->selfTestIndicator, [=]() { ui->selfTestIndicator->setStyleSheet(STYLE_INDICATOR_FAIL); });
    connect(aed, &aedModel::AED::signalPowerOff, ui->selfTestIndicator, [=]() { ui->selfTestIndicator->setStyleSheet(STYLE_INDICATOR_OFF); });
}

void MainWindow::buildDebugFeatures()
{
    // Just for the ECG debug buttons
    connect(ui->beginECGButton, &QPushButton::pressed, ecgModule, &aedModel::ModuleECGAssessment::startAssessment);
    connect(ui->stopECGButton, &QPushButton::pressed, ecgModule, &aedModel::ModuleECGAssessment::endAssessment);
    ui->beginECGButton->setEnabled(true);
    ui->stopECGButton->setEnabled(true);

    //    Dummy functions are for testing that certain connections are working (since we cant easily test functions via the GUI as the overall flow isnt working)
    //    aed->dummy();
    //    cprHelpModule->dummy();
    //    shockModule->dummy();
}

// TODO SLOT?
void MainWindow::turnoff()
{
    ui->lamp_CheckResp->turnOff();
    ui->lamp_CallHelp->turnOff();;
    ui->lamp_AttPads->turnOff();
    ui->lamp_Analysing->turnOff();
    ui->lamp_CPR->turnOff();


}

// SLOT
void MainWindow::quitProgram()
{
    QApplication::quit();
}
