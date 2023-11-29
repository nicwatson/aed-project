#include "AED.h"

using namespace aedModel;

AED::AED() : AED(1)
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
    //connect(module, SIGNAL(signalResult(testResult_t)), this, SLOT(selfTestResult(ModuleSelfTest::testResult_t)));
    //connect(this, SIGNAL(signalStartTest()), module, SLOT(doSelfTest()));
    return true;
}

bool AED::addModuleStartupAdvice(ModuleStartupAdvice * module)
{
    if(module == nullptr) return false;
    //connect(module, SIGNAL(sendUserPrompt(const QString &)), this, SLOT(userPrompt(const QString &)));
    //connect(this, SIGNAL(signalStartupAdvice(cableState_t)), module, SLOT(startAdvice(cableState_t)));
    //connect(this, SIGNAL(signalPadsAttached()), module, SLOT(stop()));
    return true;
}

bool AED::addModuleECG(ModuleECGAssessment * module)
{
    if(module == nullptr) return false;
    //connect(this, SIGNAL(signalStartECG()), module, SLOT(startAssessment()));
    //connect(this, SIGNAL(signalEndECG()), module, SLOT(endAssessment()));
    //connect(module, SIGNAL(signalResult(bool)), this, SLOT(ecgResult(bool)));
    return true;
}

bool AED::addModuleShock(ModuleShock * module)
{
    if(module == nullptr) return false;

    return true;
}

bool AED::addModuleCPR(ModuleCPRHelp * module)
{
    if(module == nullptr) return false;

    return true;
}
