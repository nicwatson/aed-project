#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    qDebug() << "[ENTRY] Setup / MainWindow::MainWindow() (constructor)" << Qt::endl;

    // Extra debug features disabled by default.
    // #define BUILD_DEBUG in mainwindow.h to enable them
    debug = false;
    #ifdef BUILD_DEBUG
    debug = true;
    #endif // BUILD_DEBUG

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

    // (Build debug features -- actual effect is controlled by whether BUILD_DEBUG is defined in mainwindow.h)
    buildDebugFeatures();

    // 5. ADD MODULES TO AED
    aed->addModuleSelfTest(selfTestModule);
    aed->addModuleStartupAdvice(startupAdviceModule);
    aed->addModuleECG(ecgModule);
    aed->addModuleShock(shockModule);
    aed->addModuleCPR(cprHelpModule);

    // Start with everything turned off
    turnoff();

    qDebug() << "[EXIT] Setup / MainWindow::MainWindow() (constructor)" << Qt::endl;
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTRUCTOR] MainWindow - started" << Qt::endl;

    delete ui;

    // Cleanup
    if (lcdDisplay != nullptr)          lcdDisplay->deleteLater();
    if (selfTestModule != nullptr)      selfTestModule->deleteLater();
    if (startupAdviceModule != nullptr) startupAdviceModule->deleteLater();
    if (ecgModule != nullptr)           ecgModule->deleteLater();
    if (shockModule != nullptr)         shockModule->deleteLater();
    if (cprHelpModule != nullptr)       cprHelpModule->deleteLater();
    if (aed != nullptr)                 aed->deleteLater();

    qDebug() << "[DESTRUCTOR] MainWindow - done" << Qt::endl;
}

void MainWindow::prepareLampWidgets()
{
     qDebug() << "[ENTRY] Setup / MainWindow::prepareLampWidgets()" << Qt::endl;

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
    ui->shockButton->    setFlashTime  (FLASH_TIME_SHOCK);

    repaint();
}

void MainWindow::buildModules()
{
    qDebug() << "[ENTRY] Setup / MainWindow::buildModules()" << Qt::endl;

    // Instantiate LCDDisplay and all modules
    aedGui::LCDDisplayParams params = {
        .lcdDisplayFrame =      ui->LCDDisplay,
        .ecgGraph =             ui->ecgGraph,
        .promptLabel =          ui->lcdPrompt,
        .helpLabel =            ui->lcdCPRHelp,
        .shockCounter =         ui->lcdShocks,
        .timer =                ui->lcdTimer,
        .compressionDepthBar =  ui->compressionDepthBar
    };
    lcdDisplay = new aedGui::LCDDisplay(params);

    // Instantiate all modules belonging to the AED model
    selfTestModule =        new aedModel::ModuleSelfTest();
    startupAdviceModule =   new aedModel::ModuleStartupAdvice(ui->lamp_CheckResp, ui->lamp_CallHelp, ui->lamp_AttPads);
    shockModule =           new aedModel::ModuleShock();
    ecgModule =             new aedModel::ModuleECGAssessment(lcdDisplay);
    cprHelpModule =         new aedModel::ModuleCPRHelp();
}

void MainWindow::buildModuleConnections()
{
    qDebug() << "[ENTRY] Setup / MainWindow::buildModuleConnections()" << Qt::endl;

    //
    // CONNECTING GUI ELEMENTS TO ModuleECGAssessment
    //
    connect(ui->tachyButton,        &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_TACHY);} );
    connect(ui->fibButton,          &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::VENT_FIB);} );
    connect(ui->nonShockableButton, &QPushButton::pressed, ecgModule, [=]() {ecgModule->setRhythm(aedModel::ModuleECGAssessment::NON_SHOCKABLE);} );


    //
    // CONNECTING GUI ELEMENTS TO ModuleShock (and vice-versa)
    //

    /*
    // TODO REMOVE THIS
    // Lights up the Shock button when pressed. [[Old Checked- working]]
    connect(ui->shockButton, &QPushButton::pressed, shockModule, [=]() {
        shockModule->shockButtonPressed();
        ui->shockButton->turnOn();
    });

    // Darkens the Shock button when released. [[Old Checked- working]]
    connect(ui->shockButton, &QPushButton::released, shockModule, [=]() {
        shockModule->shockButtonReleased();
        ui->shockButton->turnOff();
    });*/

    // Lets the shock button send press/release events to the AED's shock module
    connect(ui->shockButton, &QPushButton::pressed, shockModule, &aedModel::ModuleShock::shockButtonPressed);
    connect(ui->shockButton, &QPushButton::released, shockModule, &aedModel::ModuleShock::shockButtonReleased);

    // Makes the Shock Button start to flash when it's charged/ready. [[Old Checked- working]]
    connect(shockModule, &aedModel::ModuleShock::signalCharged, ui->shockButton, &aedGui::LampButton::startFlash);

    // Makes the Shock Button stop flashing when shock is delivered or aborted. [[Old Checked- working]]
    connect(shockModule, &aedModel::ModuleShock::signalAborted, ui->shockButton, &aedGui::LampButton::stopFlash);
    connect(shockModule, &aedModel::ModuleShock::signalShockDelivered, [=]() { ui->shockButton->stopFlash(); } );

    // Makes delivered shock update the LCD's shock counter
    connect(shockModule, &aedModel::ModuleShock::signalShockDelivered, lcdDisplay, &aedGui::LCDDisplay::setShockCounter);


    //
    // CONNECTING GUI ELEMENTS TO ModuleCPRHelp
    //

    // Makes the GUI sliders for CPR depth and rate update the model's ModuleCPRHelp
    connect(ui->cprDepthSlider, &QSlider::valueChanged, cprHelpModule, &aedModel::ModuleCPRHelp::updateCompressionDepth);
    connect(ui->cprRateSlider,  &QSlider::valueChanged, cprHelpModule, &aedModel::ModuleCPRHelp::updateCompressionRate);


    // Makes toggling the start/stop compressions button start/stop compressions, and instantly update the ModuleCPRHelp with the latest slider values

    connect(ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::signalButtonToggleCompressions, cprHelpModule, &aedModel::ModuleCPRHelp::toggleCompressions);

    connect(ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::signalButtonToggleCompressions, [=]() {
        cprHelpModule->updateCompressionDepth(ui->cprDepthSlider->value());
        cprHelpModule->updateCompressionRate(ui->cprRateSlider->value());
    });

    // Connections to enable/disable/reset the start/stop compressions button for when CPR phase starts and ends
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRStarted, ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::enableCPR);
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRComplete, ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::disableCPR);
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRAborted, ui->toggleCompressionsButton, &aedGui::CompressionsToggleButton::disableCPR);

    // Connects the CPR-specific secondary LCD prompt to its dedicated signal in ModuleCPRHelp
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalCPRCompressionRatePrompt, ui->lcdCPRHelp, [this](QString msg) {ui->lcdCPRHelp->setText(msg); }); // checked- working

    // Connects the compression depth indicator bar in the LCD to a signal from ModuleCPRHelp that will keep it updated
    connect(cprHelpModule, &aedModel::ModuleCPRHelp::signalDisplayCompressionDepth, ui->compressionDepthBar, [this](int val) {
        ui->compressionDepthBar->setValue(val);
    }); // [[OLD Checked- working]]


}

void MainWindow::buildAEDConnections()
{
    qDebug() << "[ENTRY] Setup / MainWindow::buildAEDConnections()" << Qt::endl;

    //
    // CONNECT GUI SIGNALS TO AED SLOTS
    //

    connect(ui->powerButton,            &QPushButton::pressed,  aed, &aedModel::AED::togglePowerButton);
    connect(ui->adultPadsButton,        &QRadioButton::clicked, aed, &aedModel::AED::plugCableAdult);
    connect(ui->pediatricPadsButton,    &QRadioButton::clicked, aed, &aedModel::AED::plugCableChild);
    connect(ui->unpluggedCablesButton,  &QRadioButton::clicked, aed, &aedModel::AED::unplugCable);
    connect(ui->rechargeBatteryButton,  &QPushButton::pressed,  aed, &aedModel::AED::changeBatteries);
    connect(ui->attachedPadsButton,     &QRadioButton::clicked, [=]() {aed->attachPads(true); } ); // [[OLD checked- working]]
    connect(ui->detachedPadsButton,     &QRadioButton::clicked, [=]() {aed->attachPads(false);} ); // [[OLD checked- working]]


    //
    // CONNECT AED SIGNALS TO GUI SLOTS
    //

    // Connect the AED's main user advice prompt signal to the LCD
    connect(aed, &aedModel::AED::signalUserPrompt, lcdDisplay, &aedGui::LCDDisplay::setPromptLabel); // [[OLD checked- working]]

    // Connect AED signals to those pictogram lamps that are not linked to the initial startup advice
    connect(aed, &aedModel::AED::signalStartLampCPR,        ui->lamp_CPR,       &aedGui::LampWidget::startFlash); // [[OLD checked- working]]
    connect(aed, &aedModel::AED::signalStopLampCPR,         ui->lamp_CPR,       &aedGui::LampWidget::stopFlash); // [[OLD checked- working]]
    connect(aed, &aedModel::AED::signalStartLampStandback,  ui->lamp_Analysing, &aedGui::LampWidget::startFlash); // [[OLD checked- working]]
    connect(aed, &aedModel::AED::signalStopLampStandback,   ui->lamp_Analysing, &aedGui::LampWidget::stopFlash); // [[OLD checked- working]]

    // Connect AED power/test/battery status changes to GUI indicators
    connect(aed, &aedModel::AED::signalBatteryChanged,  ui->batteryPercentLabel, [this](double newBatt) {
        ui->batteryPercentLabel->setText(QString("%1").arg((int) (newBatt * 100))); }); // [[OLD checked- working]]
    connect(aed, &aedModel::AED::signalStartTest,       [this](aedModel::AED*) {
        lcdDisplay->startLCD();} ); // [[OLD checked- working]]
    connect(aed, &aedModel::AED::signalPowerOff,        lcdDisplay, &aedGui::LCDDisplay::endLCD);
    connect(aed, &aedModel::AED::signalDisplayPassTest, ui->selfTestIndicator, [=]() { ui->selfTestIndicator->setStyleSheet(STYLE_INDICATOR_PASS); });
    connect(aed, &aedModel::AED::signalUnitFailed,      ui->selfTestIndicator, [=]() { ui->selfTestIndicator->setStyleSheet(STYLE_INDICATOR_FAIL); });
    connect(aed, &aedModel::AED::signalPowerOff,        ui->selfTestIndicator, [=]() { ui->selfTestIndicator->setStyleSheet(STYLE_INDICATOR_OFF); });
}


void MainWindow::turnoff()
{
    qDebug() << "[ENTRY] Setup / MainWindow::turnoff()" << Qt::endl;

    ui->lamp_CheckResp->turnOff();
    ui->lamp_CallHelp->turnOff();;
    ui->lamp_AttPads->turnOff();
    ui->lamp_Analysing->turnOff();
    ui->lamp_CPR->turnOff();
    ui->shockButton->turnOff();

    qDebug() << "[EXIT] Setup / MainWindow::turnoff() -- all indicator lamps should now be off" << Qt::endl;
}

void MainWindow::buildDebugFeatures()
{
    qDebug().noquote().nospace()  << "[ENTRY] Setup / MainWindow::buildDebugFeatures() [Extra debug features are "
                                  << (debug ? "enabled" : "disabled") << "]" << Qt::endl;

    if(debug)
    {
        // Just for the ECG debug buttons
        connect(ui->beginECGButton, &QPushButton::pressed, ecgModule, &aedModel::ModuleECGAssessment::startAssessment);
        connect(ui->stopECGButton,  &QPushButton::pressed, ecgModule, &aedModel::ModuleECGAssessment::endAssessment);
        ui->beginECGButton->setEnabled(true);
        ui->stopECGButton->setEnabled(true);
        ui->beginECGButton->show();
        ui->stopECGButton->show();

        //    Dummy functions are for testing that certain connections are working (since we cant easily test functions via the GUI as the overall flow isnt working)
        //    aed->dummy();
        //    cprHelpModule->dummy();
        //    shockModule->dummy();
    }
    else
    {
        ui->beginECGButton->hide();
        ui->stopECGButton->hide();
    }

    qDebug() << "[EXIT] Setup / MainWindow::buildDebugFeatures()";
}
