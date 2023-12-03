// AED class - setup functions

#include "AED.h"
#include "ModuleStartupAdvice.h"
#include "ModuleECGAssessment.h"
#include "ModuleShock.h"
#include "ModuleCPRHelp.h"

using namespace aedModel;

AED::AED() : AED(1.0)
{

}

AED::AED(float initBatt) : QObject(), aedState(OFF), cableState(PAD_ADULT), padsAttached(false), battery(initBatt)
{

}

AED::~AED()
{

}

bool AED::addModuleSelfTest(ModuleSelfTest * module)
{
    if(module == nullptr) return false;
    connect(module, &aedModel::ModuleSelfTest::signalResult, this, &aedModel::AED::selfTestResult);
    // connect(module, SIGNAL(signalResult(ModuleSelfTest::testResult_t)), this, SLOT(selfTestResult(ModuleSelfTest::testResult_t)));
    connect(this, SIGNAL(signalStartTest(AED *)), module, SLOT(startSelfTest(AED *)));
    connect(this, SIGNAL(signalAbortSelfTest()), module, SLOT(abortSelfTest()));
    connect(this, SIGNAL(signalAbortAll()), module, SLOT(abortSelfTest()));
    return true;
}

bool AED::addModuleStartupAdvice(ModuleStartupAdvice * module)
{

    if(module == nullptr) return false;
    connect(module, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    connect(this, &aedModel::AED::signalStartupAdvice, module, &aedModel::ModuleStartupAdvice::startAdvice);
//    connect(this, SIGNAL(signalStartupAdvice(AED::cableState_t)), module, SLOT(startAdvice(AED::cableState_t)));
    connect(this, SIGNAL(signalPadsAttached()), module, SLOT(stop()));
    connect(this, SIGNAL(signalAbortStartupAdvice()), module, SLOT(stop()));
    connect(this, SIGNAL(signalAbortAll()), module, SLOT(stop()));
    return true;
}

bool AED::addModuleECG(ModuleECGAssessment * module)
{
    if(module == nullptr) return false;
    connect(module, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    connect(module, SIGNAL(signalResult(bool)), this, SLOT(ecgResult(bool)));
    connect(this, SIGNAL(signalStartECG()), module, SLOT(startAssessment()));
    connect(this, SIGNAL(signalAbortECG()), module, SLOT(endAssessment()));
    connect(this, SIGNAL(signalAbortAll()), module, SLOT(endAssessment()));
    return true;
}

bool AED::addModuleShock(ModuleShock * module)
{
    if(module == nullptr) return false;
    connect(module, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    connect(module, SIGNAL(signalDone()), this, SLOT(shockDelivered()));
    connect(module, SIGNAL(signalDrainBatt(double)), this, SLOT(useBattery(double)));
    connect(this, SIGNAL(signalPrepShock(bool)), module, SLOT(start(bool)));
    connect(this, SIGNAL(signalAbortShock()), module, SLOT(abort()));
    connect(this, SIGNAL(signalAbortAll()), module, SLOT(abort()));
    connect(this, SIGNAL(signalPowerOff()), module, SLOT(fullReset()));
    return true;
}

bool AED::addModuleCPR(ModuleCPRHelp * module)
{
    if(module == nullptr) return false;
    connect(module, SIGNAL(signalUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    connect(module, SIGNAL(signalCPRComplete()), this, SLOT(cprStopped()));
    connect(this, &aedModel::AED::signalStartCPR, module, &aedModel::ModuleCPRHelp::start);
    // connect(this, SIGNAL(signalStartCPR(cableState_t)), module, SLOT(start(cableState_t)));
    connect(this, SIGNAL(signalAbortCPR()), module, SLOT(abort()));
    connect(this, SIGNAL(signalAbortAll()), module, SLOT(abort()));
    return true;
}
