// FILE: AED_setup.cpp
// AED class - setup functions

#include "AED.h"
#include "ModuleStartupAdvice.h"
#include "ModuleECGAssessment.h"
#include "ModuleShock.h"
#include "ModuleCPRHelp.h"

using namespace aedModel;


// Used for debugging - translate between state_t underlying integers and readable strings
const QString AED::stateNames[7] = { "OFF", "SELF_TEST", "FAILURE", "STARTUP_ADVICE", "ECG_ASSESS", "SHOCK", "CPR"};

// Used for debugging - translate between cableState_t underlying integers and readable strings
const QString AED::cableStateNames[3] = { "UNPLUGGED", "PAD_ADULT", "PAD_CHILD" };

AED::AED() : AED(1.0)
{
    qDebug().noquote().nospace() << "[CONSTRUCTOR] AED()" << Qt::endl;
}

AED::AED(float initBatt) : QObject(), aedState(OFF), cableState(PAD_ADULT), padsAttached(false), battery(initBatt)
{
    qDebug().noquote().nospace()
            << "[CONSTRUCTOR] AED("
            << "initBatt = " << initBatt << ")" << Qt::endl;
}

AED::~AED()
{
    qDebug().noquote().nospace() << "[DESTRUCTOR] AED" << Qt::endl;
    // No dynamic allocations to destroy
}

bool AED::addModuleSelfTest(ModuleSelfTest * module)
{
    qDebug() << "[ENTRY] AED::addModuleSelfTest(ModuleSelfTest *)" << Qt::endl
             << "\tIf no [EXIT], arg was nullptr" << Qt::endl;

    if(module == nullptr) return false;

    connect(module, &ModuleSelfTest::signalResult, this, &AED::selfTestResult);

    connect(this, &AED::signalStartTest,     module, &ModuleSelfTest::startSelfTest);
    connect(this, &AED::signalAbortSelfTest, module, &ModuleSelfTest::abort);

    qDebug() << "[EXIT] AED::addModuleSelfTest(ModuleSelfTest *)" << Qt::endl;
    return true;
}

bool AED::addModuleStartupAdvice(ModuleStartupAdvice * module)
{
    qDebug() << "[ENTRY] AED::addModuleStartupAdvice(ModuleStartupAdvice *)" << Qt::endl
             << "\tIf no [EXIT], arg was nullptr" << Qt::endl;

    if(module == nullptr) return false;

    connect(module, &ModuleStartupAdvice::signalForwardUserPrompt, this, &AED::userPrompt);

    connect(this, &AED::signalStartupAdvice,      module, &ModuleStartupAdvice::startAdvice);
    connect(this, &AED::signalPadsAttached,       module, &ModuleStartupAdvice::stop);
    connect(this, &AED::signalAbortStartupAdvice, module, &ModuleStartupAdvice::stop);

    qDebug() << "[EXIT] AED::addModuleStartupAdvice(ModuleStartupAdvice *)" << Qt::endl;
    return true;
}

bool AED::addModuleECG(ModuleECGAssessment * module)
{
    qDebug() << "[ENTRY] AED::addModuleECG(ModuleECGAssessment *)" << Qt::endl
             << "\tIf no [EXIT], arg was nullptr" << Qt::endl;

    if(module == nullptr) return false;

    connect(module, &ModuleECGAssessment::signalForwardUserPrompt, this, &AED::userPrompt);
    connect(module, &ModuleECGAssessment::signalResult,     this, &AED::ecgResult);

    connect(this, &AED::signalStartECG, module, &ModuleECGAssessment::startAssessment);
    connect(this, &AED::signalAbortECG, module, &ModuleECGAssessment::endAssessment);

    qDebug() << "[EXIT] AED::addModuleECG(ModuleECGAssessment *)" << Qt::endl;
    return true;
}

bool AED::addModuleShock(ModuleShock * module)
{
    qDebug() << "[ENTRY] AED::addModuleShock(ModuleShock *)" << Qt::endl
             << "\tIf no [EXIT], arg was nullptr" << Qt::endl;

    if(module == nullptr) return false;

    connect(module, &ModuleShock::signalForwardUserPrompt, this, &AED::userPrompt);
    connect(module, &ModuleShock::signalDrainBatt,  this, &AED::useBattery);
    connect(module, &ModuleShock::signalDone,       this, [=]() { this->shockDelivered(); } );

    connect(this, &AED::signalPrepShock,  module, &ModuleShock::start);
    connect(this, &AED::signalAbortShock, module, &ModuleShock::abort);
    connect(this, &AED::signalPowerOff,   module, &ModuleShock::fullReset);

    qDebug() << "[EXIT] AED::addModuleShock(ModuleShock *)" << Qt::endl;
    return true;
}

bool AED::addModuleCPR(ModuleCPRHelp * module)
{
    qDebug() << "[ENTRY] AED::addModuleCPR(ModuleCPRHelp *)" << Qt::endl
             << "\tIf no [EXIT], arg was nullptr" << Qt::endl;

    if(module == nullptr) return false;

    connect(module, &ModuleCPRHelp::signalForwardUserPrompt,  this, &AED::userPrompt);
    connect(module, &ModuleCPRHelp::signalCPRComplete, this, &AED::cprStopped);

    connect(this, &AED::signalStartCPR, module, &ModuleCPRHelp::start);
    connect(this, &AED::signalAbortCPR, module, &ModuleCPRHelp::abort);

    qDebug() << "[EXIT] AED::addModuleCPR(ModuleCPRHelp *)" << Qt::endl;
    return true;
}

void AED::dummy()
{
    qDebug() << "[ENTRY] AED::dummy()" << Qt::endl;

    emit signalBatteryChanged(0.255);
    emit signalStartLampStandback();
    emit signalStartLampCPR();

    QTimer::singleShot(5000, this, [=]() {
        emit signalUserPrompt("dumbodore");
        emit signalStopLampStandback();
        emit signalStopLampCPR();
    });

    qDebug() << "[EXIT] AED::dummy()" << Qt::endl;
}
